#pragma once
#include <string>

namespace OS
{
	class PathParser
	{
	public:
		enum class FileType
		{
			UNKNOWN,
			MODEL,
			TEXTURE,
			SHADER,
			MATERIAL,
			SOUND,
			SCENE,
			SCRIPT,
			FONT
		};

		PathParser() = delete;

		static std::string MakeWindowsStyle(const std::string& path);

		static std::string MakeNonWindowsStyle(const std::string& path);

		static std::string GetContainingFolder(const std::string& path);

		static std::string GetElementName(const std::string& path);

		static std::string GetExtension(const std::string& path);

		static std::string FileTypeToString(FileType fileType);

		static FileType GetFileType(const std::string& path);
	};

	class SystemCalls
	{
	public:
		SystemCalls() = delete;

		static void ShowInExplorer(const std::string& path);

		static void OpenFile(const std::string& file, const std::string& workingDir = "");

		static void EditFile(const std::string& file);

		static void OpenURL(const std::string& url);
	};
}