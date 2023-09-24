#include "D3D12EraHubRenderer.h"
#include <imgui.h>
#include "SystemCalls.h"
#include "Debug.h"

bool window = true;
bool adding = false;
std::string tempname{ "Enter project name.." };

D3D12EraHubRenderer::D3D12EraHubRenderer()
{
    m_CoreHub = new EraHubCore();
    m_CoreHub->ParseProjects();
    LOG_INFO("[Hub] Initialized successfuly");
}

D3D12EraHubRenderer::~D3D12EraHubRenderer()
{
    if (m_CoreHub)
    {
        delete m_CoreHub;
        m_CoreHub = nullptr;
    }
}

void D3D12EraHubRenderer::Render()
{
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Github"))
            {
                OS::SystemCalls::OpenURL("https://github.com/EldarMuradov/EraEngine");
            }
            if (ImGui::MenuItem("Tutorial"))
            {
                OS::SystemCalls::OpenURL("https://github.com/EldarMuradov/EraEngine");
            }
            if (ImGui::MenuItem("Bug Report"))
            {
                OS::SystemCalls::OpenURL("https://github.com/EldarMuradov/EraEngine");
            }
            ImGui::Separator();
            ImGui::Text("v0.0524 - <DX12>");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Projects", &window, ImGuiWindowFlags_NoMove);
    ImGui::SetWindowSize(ImVec2(1280, 800));
    ImGui::SetWindowPos(ImVec2(0, 0));
    if (ImGui::Button("Add Project"))
        adding = !adding;

    if (adding)
    {
        ImGui::InputText("Project name: ", ((char*)tempname.c_str()), 255);
        ImGui::SameLine();
        if (ImGui::Button("Create Project"))
        {
            OS::SystemCalls::ShowInExplorer("");
            LOG_INFO("[Hub] Project added");
        }
    }
    ImGui::Separator();

    ImGui::Text("All projects:");
    const auto& projects = *m_CoreHub->GetProjects();
    if (projects.size() > 0)
    {
        for (auto& proj : projects)
        {
            proj.second->Render();
        }
    }

    ImGui::End();
}