#pragma once
#include <vector>
#include "Project.h"
#include <unordered_map>
#include <string>
#include <mutex>

class EraHubCore
{
public:
	void ParseProjects();

	void AddProject(Project* project) { m_Projects.emplace(std::make_pair(std::string(project->Name), project)); }

	void RemoveProject(std::string name) { m_Projects.erase(name); }

	void SaveChanges();

	static void OpenProject(Project* project);

	constexpr std::unordered_map<std::string, Project*>* GetProjects() noexcept { return &m_Projects; }

	static void CreateProjectInFolder(Project* project);

private:
	std::unordered_map<std::string, Project*> m_Projects;

	std::mutex m_SyncMutex;
};