#pragma once
#include "Include/stdafx.h"
#include <imgui.h>
#include "D3D12EraHub.h"
#include "D3D12EraHubRenderer.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class EraHubWin32App
{
public:
	EraHubWin32App();
	~EraHubWin32App();

	void Init();

private:
	void Run(HWND hWnd, WNDCLASSEXW wc);

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	D3D12EraHubRenderer m_HubRenderer;
};