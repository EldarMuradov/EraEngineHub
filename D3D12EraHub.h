#pragma once

#include "Include/stdafx.h"
#include "GUIHubRenderer.h"

using Microsoft::WRL::ComPtr;

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64 FenceValue;
};

class D3D12EraHub
{
public:
	D3D12EraHub();
	~D3D12EraHub();

private:
	void Init(HWND hWnd, WNDCLASSEXW wc);

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void WaitForLastSubmittedFrame();
	FrameContext* WaitForNextFrameResources();

private:
	int const NUM_BACK_BUFFERS = 3;
	int const NUM_FRAMES_IN_FLIGHT = 3;

	FrameContext m_FrameContext[3] = {};

	UINT m_FrameIndex = 0;

	HANDLE m_FenceEvent = NULL;
	HANDLE m_SwapChainWaitableObject = NULL;

	UINT64 m_FenceLastSignaledValue = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE m_MainRenderTargetDescriptor[3] = {};

	ID3D12Device* m_D3DDevice = NULL;

	ID3D12DescriptorHeap* m_D3DRtvDescHeap = NULL;
	ID3D12DescriptorHeap* m_D3DSrvDescHeap = NULL;

	ID3D12CommandQueue* m_D3DCommandQueue = NULL;

	ID3D12GraphicsCommandList* m_D3DCommandList = NULL;

	ID3D12Fence* m_Fence = NULL;

	IDXGISwapChain3* m_SwapChain = NULL;

	ID3D12Resource* m_MainRenderTargetResource[3] = {};

	friend class EraHubWin32App;
	friend void GUIHubRenderer::EndFrame();
};