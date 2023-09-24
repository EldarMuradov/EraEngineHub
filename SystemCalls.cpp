#include "SystemCalls.h"
#include <Windows.h>
#include <filesystem>

void OS::SystemCalls::ShowInExplorer(const std::string& path)
{
	ShellExecuteA(NULL, "open", PathParser::MakeWindowsStyle(path).c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void OS::SystemCalls::OpenFile(const std::string& file, const std::string& workingDir)
{
	ShellExecuteA(NULL, "open", PathParser::MakeWindowsStyle(file).c_str(), NULL,
		workingDir.empty() ? NULL : PathParser::MakeWindowsStyle(workingDir).c_str(),
		SW_SHOWNORMAL);
}

void OS::SystemCalls::EditFile(const std::string& file)
{
	ShellExecuteW(NULL, NULL, std::wstring(file.begin(), file.end()).c_str(), NULL, NULL, SW_NORMAL);
}

void OS::SystemCalls::OpenURL(const std::string& url)
{
	ShellExecuteA(0, 0, url.c_str(), 0, 0, SW_SHOW);
}

std::string OS::PathParser::MakeWindowsStyle(const std::string& path)
{
	std::string result;
	result.resize(path.size());

	for (size_t i = 0; i < path.size(); ++i)
		result[i] = path[i] == '/' ? '\\' : path[i];

	return result;
}

std::string OS::PathParser::MakeNonWindowsStyle(const std::string& path)
{
	std::string result;
	result.resize(path.size());

	for (size_t i = 0; i < path.size(); ++i)
		result[i] = path[i] == '\\' ? '/' : path[i];

	return result;
}

std::string OS::PathParser::GetContainingFolder(const std::string& path)
{
	std::string result;

	bool extraction = false;

	for (auto it = path.rbegin(); it != path.rend(); ++it)
	{
		if (extraction)
			result += *it;

		if (!extraction && it != path.rbegin() && (*it == '\\' || *it == '/'))
			extraction = true;
	}

	std::reverse(result.begin(), result.end());

	if (!result.empty() && result.back() != '\\')
		result += '\\';

	return result;
}

std::string OS::PathParser::GetElementName(const std::string& path)
{
	std::string result;

	std::string tpath = path;
	if (!tpath.empty() && tpath.back() == '\\')
		tpath.pop_back();

	for (auto it = tpath.rbegin(); it != tpath.rend() && *it != '\\' && *it != '/'; ++it)
		result += *it;

	std::reverse(result.begin(), result.end());

	return result;
}

std::string OS::PathParser::GetExtension(const std::string& path)
{
	std::string result;

	for (auto it = path.rbegin(); it != path.rend() && *it != '.'; ++it)
		result += *it;

	std::reverse(result.begin(), result.end());

	return result;
}

std::string OS::PathParser::FileTypeToString(FileType fileType)
{
	switch (fileType)
	{
	case OS::PathParser::FileType::MODEL:		return "Model";
	case OS::PathParser::FileType::TEXTURE:		return "Texture";
	case OS::PathParser::FileType::SHADER:		return "Shader";
	case OS::PathParser::FileType::MATERIAL:	return "Material";
	case OS::PathParser::FileType::SOUND:		return "Sound";
	case OS::PathParser::FileType::SCENE:		return "Scene";
	case OS::PathParser::FileType::SCRIPT:		return "Script";
	case OS::PathParser::FileType::FONT:		return "Font";
	}

	return "Unknown";
}

OS::PathParser::FileType OS::PathParser::GetFileType(const std::string& path)
{
	std::string ext = GetExtension(path);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if (ext == "fbx" || ext == "obj")											return FileType::MODEL;
	else if (ext == "png" || ext == "jpeg" || ext == "jpg" || ext == "tga")		return FileType::TEXTURE;
	else if (ext == "glsl")														return FileType::SHADER;
	else if (ext == "ovmat")													return FileType::MATERIAL;
	else if (ext == "wav" || ext == "mp3" || ext == "ogg")						return FileType::SOUND;
	else if (ext == "ovscene")													return FileType::SCENE;
	else if (ext == "lua")														return FileType::SCRIPT;
	else if (ext == "ttf")														return FileType::FONT;

	return FileType::UNKNOWN;
}