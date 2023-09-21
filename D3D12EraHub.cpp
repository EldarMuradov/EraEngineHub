#include "D3D12EraHub.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "GUIHubRenderer.h"

D3D12EraHub::D3D12EraHub()
{
}

D3D12EraHub::~D3D12EraHub()
{
}

void D3D12EraHub::Init(HWND hWnd, WNDCLASSEXW wc)
{
    if (!CreateDeviceD3D(hWnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    GUIHubRenderer::SetTheme(GUIHubRenderer::Theme::Black);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX12_Init(m_D3DDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, m_D3DSrvDescHeap,
        m_D3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        m_D3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
}

bool D3D12EraHub::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = NULL;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&m_D3DDevice)) != S_OK)
        return false;

#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != NULL)
    {
        ID3D12InfoQueue* pInfoQueue = NULL;
        g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (m_D3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_D3DRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_D3DRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (size_t i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            m_MainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (m_D3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_D3DSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (m_D3DDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_D3DCommandQueue)) != S_OK)
            return false;
    }

    for (size_t i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (m_D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (m_D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&m_D3DCommandList)) != S_OK ||
        m_D3DCommandList->Close() != S_OK)
        return false;

    if (m_D3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)) != S_OK)
        return false;

    m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_FenceEvent == NULL)
        return false;

    {
        IDXGIFactory4* dxgiFactory = nullptr;
        IDXGISwapChain1* swapChain1 = nullptr;

        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;

        if (dxgiFactory->CreateSwapChainForHwnd(m_D3DCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
            return false;

        if (swapChain1->QueryInterface(IID_PPV_ARGS(&m_SwapChain)) != S_OK)
            return false;

        swapChain1->Release();
        dxgiFactory->Release();
        m_SwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        m_SwapChainWaitableObject = m_SwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void D3D12EraHub::CleanupDeviceD3D()
{
    CleanupRenderTarget();

    if (m_SwapChain) 
    { 
        m_SwapChain->SetFullscreenState(false, NULL); 
        m_SwapChain->Release(); 
        m_SwapChain = NULL; 
    }

    if (m_SwapChainWaitableObject != NULL) 
     CloseHandle(m_SwapChainWaitableObject); 

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
    {
        if (m_FrameContext[i].CommandAllocator)
        {
            m_FrameContext[i].CommandAllocator->Release();
            m_FrameContext[i].CommandAllocator = NULL;
        }
    }

    if (m_D3DCommandQueue) 
    { 
        m_D3DCommandQueue->Release(); 
        m_D3DCommandQueue = NULL; 
    }

    if (m_D3DCommandList) 
    { 
        m_D3DCommandList->Release(); 
        m_D3DCommandList = NULL; 
    }

    if (m_D3DRtvDescHeap) 
    { 
        m_D3DRtvDescHeap->Release(); 
        m_D3DRtvDescHeap = NULL; 
    }

    if (m_D3DSrvDescHeap) 
    { 
        m_D3DSrvDescHeap->Release(); 
        m_D3DSrvDescHeap = NULL; 
    }

    if (m_Fence) 
    { 
        m_Fence->Release(); 
        m_Fence = NULL; 
    }

    if (m_FenceEvent) 
    { 
        CloseHandle(m_FenceEvent); 
        m_FenceEvent = NULL; 
    }

    if (m_D3DDevice) 
    { 
        m_D3DDevice->Release(); 
        m_D3DDevice = NULL; 
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = NULL;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void D3D12EraHub::CreateRenderTarget()
{
    for (size_t i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = NULL;
        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        m_D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, m_MainRenderTargetDescriptor[i]);
        m_MainRenderTargetResource[i] = pBackBuffer;
    }
}

void D3D12EraHub::CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (size_t i = 0; i < NUM_BACK_BUFFERS; i++)
        if (m_MainRenderTargetResource[i]) { m_MainRenderTargetResource[i]->Release(); m_MainRenderTargetResource[i] = NULL; }
}

void D3D12EraHub::WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &m_FrameContext[m_FrameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return;

    frameCtx->FenceValue = 0;
    if (m_Fence->GetCompletedValue() >= fenceValue)
        return;

    m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
    WaitForSingleObject(m_FenceEvent, INFINITE);
}

FrameContext* D3D12EraHub::WaitForNextFrameResources()
{
    UINT nextFrameIndex = m_FrameIndex + 1;
    m_FrameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { m_SwapChainWaitableObject, NULL };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &m_FrameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0)
    {
        frameCtx->FenceValue = 0;
        m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
        waitableObjects[1] = m_FenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}