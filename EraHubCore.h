#pragma once
#include <vector>
#include "Project.h"
#include <unordered_map>
#include <string>

class EraHubCore
{
public:
	void ParseProjects();

	void AddProject(Project* project) { m_Projects.emplace(std::make_pair(project->Name, project)); }

	void RemoveProject(std::string name) { m_Projects.erase(name); }

	constexpr std::unordered_map<std::string, Project*>* GetProjects() noexcept { return &m_Projects; }

private:
	std::unordered_map<std::string, Project*> m_Projects;
};