#include "EraHubWin32App.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include "GUIHubRenderer.h"
#include "resource.h"

D3D12EraHub* Hub = nullptr;

EraHubWin32App::EraHubWin32App()
{
}

EraHubWin32App::~EraHubWin32App()
{
}

void EraHubWin32App::Init()
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"EraHub", NULL };
    wc.hIcon = static_cast<HICON>(LoadImageA(wc.hInstance, MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
    wc.hIconSm = static_cast<HICON>(LoadImageA(wc.hInstance, MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
    
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Era Engine Hub", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    Hub = new D3D12EraHub();
    Hub->Init(hwnd, wc);
    GUIHubRenderer::Init(Hub);

    m_HubRenderer = D3D12EraHubRenderer();

    Run(hwnd, wc);
}

void EraHubWin32App::Run(HWND hWnd, WNDCLASSEXW wc)
{

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        GUIHubRenderer::BeginFrame();
        
        m_HubRenderer.Render();

        GUIHubRenderer::EndFrame();
    }

    Hub->WaitForLastSubmittedFrame();

    GUIHubRenderer::Finalize();

    Hub->CleanupDeviceD3D();
    ::DestroyWindow(hWnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

LRESULT WINAPI EraHubWin32App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (Hub->m_D3DDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            Hub->WaitForLastSubmittedFrame();
            Hub->CleanupRenderTarget();
            HRESULT result = Hub->m_SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            Hub->CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
