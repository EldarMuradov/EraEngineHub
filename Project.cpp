#include "Project.h"
#include <imgui.h>
#include "EraHubCore.h"

void Project::Render()
{
	ImGui::Separator();
	ImGui::Text(Name.c_str());
	ImGui::Text(Path.c_str());
	if (ImGui::Button("Open Project"))
	{
		EraHubCore::OpenProject(this);
	}
	ImGui::Separator();
}