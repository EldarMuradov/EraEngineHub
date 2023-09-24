#pragma once
#include "EraHubCore.h"

class D3D12EraHubRenderer
{
public:
	D3D12EraHubRenderer();
	~D3D12EraHubRenderer();

	void Render();

private:
	EraHubCore* m_CoreHub = nullptr;
};