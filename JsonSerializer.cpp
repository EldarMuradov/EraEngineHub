#include "JsonSerializer.h"

std::unordered_map<std::string, Project*> JSON::JsonSerializer::DeserializeGlobalHubFile()
{
	return {};
}

void JSON::JsonSerializer::SerializeGlobalHubFile(std::unordered_map<std::string, Project*> projects)
{
}

json JSON::JsonSerializer::Merge(const json& rhs, const json& lhs)
{
	json result = rhs.flatten();
	json temp = lhs.flatten();

	json::iterator iter = temp.begin();

	for (; iter != temp.end(); ++iter)
	{
		result[iter.key()] = iter.value();
	}

	return result.unflatten();
}