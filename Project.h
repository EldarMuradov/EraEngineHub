#pragma once
#include <string>

class Project
{
public:
	void Render();

	std::string Path;
	std::string Configuration;
	std::string Name;
	std::string Version;
};