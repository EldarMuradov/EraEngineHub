#include "D3D12EraHubRenderer.h"
#include <imgui.h>
#include "SystemCalls.h"
#include "Debug.h"
#include "Builder.h"

bool window = true;
bool adding = false;
std::string tempname{ "Enter project name.." };
std::string temppath{ "Enter project path.." };

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
        Project* project = new Project();
        ImGui::InputText("Project name: ", ((char*)tempname.c_str()), 255);

        ImGui::NewLine();

        if (ImGui::BeginMenu("Project type:"))
        {
            if (ImGui::MenuItem("3D Demo template"))
            {
                project->Type = Project::ProjectType::Type_3D_Demo;
            }
            if (ImGui::MenuItem("3D Empty"))
            {
                project->Type = Project::ProjectType::Type_3D;
            }
            if (ImGui::MenuItem("2D Empty"))
            {
                project->Type = Project::ProjectType::Type_2D;
            }

            ImGui::EndMenu();
        }

        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();

        if (ImGui::Button("Create Project"))
        {
            project->Name = tempname;
            project->Path = temppath;
            m_CoreHub->AddProject(project);
            m_CoreHub->SaveChanges();
            EraHubCore::CreateProjectInFolder(project);
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