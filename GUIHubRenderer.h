#pragma once
#include "Include/stdafx.h"

class D3D12EraHub;

namespace GUIHubRenderer
{
    enum class Theme
    {
        Black = 0,
        Dark,
        Dracula,
        Grey,
        Light,
        Blue,
        ClassicLight,
        ClassicDark,
        Classic,
        Cherry,
        Cinder,
        Default
    };

    void Init(D3D12EraHub* hub);

	void SetTheme(Theme theme);
	void BeginFrame();
	void EndFrame();
	void Finalize();
};