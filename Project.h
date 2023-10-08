#pragma once
#include <string>

class Project
{
public:
	enum class ProjectType : uint8_t
	{
		Type_3D = 0,
		Type_3D_Demo = 1,
		Type_2D = 2
	};

public:
	void Render();

	std::string Path = "";
	std::string Configuration = "";
	std::string Name = "";
	ProjectType Type = ProjectType::Type_3D_Demo;
	std::string Version = "0.01";
};