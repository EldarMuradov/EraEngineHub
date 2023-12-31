#include "EraHubCore.h"
#include "JsonSerializer.h"
#include "Include/AsyncCore.h"
#include "Builder.h"

void EraHubCore::ParseProjects()
{
	m_SyncMutex.lock();
	m_Projects = JSON::JsonSerializer::DeserializeGlobalHubFile();
	m_SyncMutex.unlock();
}

void EraHubCore::SaveChanges()
{
	m_SyncMutex.lock();
	JSON::JsonSerializer::SerializeGlobalHubFile(m_Projects);
	m_SyncMutex.unlock();
}

void EraHubCore::OpenProject(Project* project)
{
}

void EraHubCore::CreateProjectInFolder(Project* project)
{
	OS::Builder::Build(project);
}