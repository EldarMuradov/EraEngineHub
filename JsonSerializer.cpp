#include "JsonSerializer.h"
#include <fstream>
#include "Project.h"
#include "Debug.h"

std::unordered_map<std::string, Project*> JSON::JsonSerializer::DeserializeGlobalHubFile()
{

	std::string filename = std::string(std::filesystem::current_path().string()) + std::string("\\Data\\EraHubMeta.ehubmeta");

	std::ifstream file(filename);

	json jprojects;
	file >> jprojects;

	int size = jprojects["Size"].get<int>();
	size_t i = 0;

	std::unordered_map<std::string, Project*> projects;

	for (; i < size; i++)
	{
		Project* proj = new Project();
		std::string name = jprojects["Name" + std::to_string(i)].get<std::string>();
		proj->Name = name;
		proj->Type = (Project::ProjectType)jprojects["Type" + std::to_string(i)].get<int>();
		proj->Path = jprojects["Path" + std::to_string(i)].get<std::string>();
		proj->Configuration = jprojects["Configuration" + std::to_string(i)].get<std::string>();
		if (!projects.count(name))
			projects.emplace(std::make_pair(name, proj));
	}

	return projects;
}

void JSON::JsonSerializer::SerializeGlobalHubFile(std::unordered_map<std::string, Project*> projects)
{
	json obj = 
	{
		{"Size", projects.size()}
	};

	size_t i = 0;

	for (auto& proj : projects)
	{
		json jproj;

		std::string reBuiltPath = proj.second->Path;
		LOG_INFO(reBuiltPath);
		std::string path = std::string(proj.second->Name);
		
		jproj = 
		{
			{"Name" + std::to_string(i), path.c_str()},
			{"Configuration" + std::to_string(i), proj.second->Configuration},
			{"Version" + std::to_string(i), proj.second->Version},
			{"Type" + std::to_string(i), proj.second->Type},
			{"Path" + std::to_string(i), reBuiltPath},
		};

		obj = Merge(obj, jproj);
		i++;
	}

	std::string filename = std::string(std::filesystem::current_path().string()) + std::string("\\Data\\EraHubMeta.ehubmeta");
	std::ofstream file(filename);
	file << obj << std::endl;
}

json JSON::JsonSerializer::Merge(const json& rhs, const json& lhs) noexcept
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