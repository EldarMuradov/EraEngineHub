#pragma once
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>

class Project;

using json = nlohmann::basic_json<>;

namespace JSON
{
	class JsonSerializer
	{
	public:
		static std::unordered_map<std::string, Project*> DeserializeGlobalHubFile();
		static void SerializeGlobalHubFile(std::unordered_map<std::string, Project*> projects);
	
	private:
		static json Merge(const json& rhs, const json& lhs) noexcept;
	};
};