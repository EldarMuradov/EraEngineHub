#include "EraHubCore.h"
#include "JsonSerializer.h"
#include "Include/AsyncCore.h"

void EraHubCore::ParseProjects()
{
	_async_t(
		m_SyncMutex.lock();
		m_Projects = JSON::JsonSerializer::DeserializeGlobalHubFile();
		m_SyncMutex.unlock();
	);
}

void EraHubCore::SaveChanges()
{
	_async_t(
		m_SyncMutex.lock();
		JSON::JsonSerializer::SerializeGlobalHubFile(m_Projects);
		m_SyncMutex.unlock();
	);
}