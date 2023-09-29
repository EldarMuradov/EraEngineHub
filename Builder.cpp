#include "Builder.h"

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <filesystem>
#include "SystemCalls.h"
#include "Debug.h"
#include <cderr.h>

std::optional<std::string> OS::Builder::SelectBuildFolder()
{
	Dialogs::SaveFileDialog dialog("Project location");
	dialog.DefineExtension("Project", "..");
	dialog.Show();
	if (dialog.HasSucceeded())
	{
		std::string result = dialog.GetSelectedFilePath();
		result = std::string(result.data(), result.data() + result.size() - std::string("..").size()) + "\\";

		if (!std::filesystem::exists(result))
			return result;
		else
		{
			LOG_ERR("[Building] Folder already exists!");
			return {};
		}
	}
	else
	{
		return {};
	}
}

void OS::Builder::Build(Project* project, bool autoRun, bool tempFolder)
{
	std::string destinationFolder;

	if (tempFolder)
	{
		destinationFolder = std::string(getenv("APPDATA")) + "\\EraHub\\Data\\ProjectTemplate\\";
		try
		{
			std::filesystem::remove_all(destinationFolder);
		}
		catch (std::filesystem::filesystem_error error)
		{
			LOG_ERR("[Building] Temporary build failed!");
			return;
		}
	}
	else if (auto res = SelectBuildFolder(); res.has_value())
		destinationFolder = res.value();
	else
		return;

	std::string buildPath(destinationFolder);

	bool failed = false;

	LOG_INFO("Preparing to build at location: \"" + buildPath + "\"");

	std::filesystem::remove_all(buildPath);
	std::error_code err;
	
	try 
	{
		std::filesystem::copy("Data\\ProjectTemplate", buildPath, std::filesystem::copy_options::recursive, err);
	}
	catch (...)
	{
		LOG_ERR("[Building] " + err.message());
	}
	
	LOG_INFO("[Building] Builded successfuly!");
}

Dialogs::SaveFileDialog::SaveFileDialog(const std::string& dialogTitle) : FileDialog(GetSaveFileNameA, dialogTitle)
{
}

void Dialogs::SaveFileDialog::Show(ExplorerFlags flags)
{
	FileDialog::Show(flags);

	if (m_Succeeded)
		AddExtensionToFilePathAndName();
}

void Dialogs::SaveFileDialog::DefineExtension(const std::string& label, const std::string& extension)
{
	m_Filter = label + '\0' + '*' + extension + '\0';
	m_Extension = extension;
}

void Dialogs::SaveFileDialog::AddExtensionToFilePathAndName()
{
	if (m_Filename.size() >= m_Extension.size())
	{
		std::string fileEnd(m_Filename.data() + m_Filename.size() - m_Extension.size(), m_Filename.data() + m_Filename.size());

		if (fileEnd != m_Extension)
		{
			m_Filepath += m_Extension;
			m_Filename += m_Extension;
		}
	}
	else
	{
		m_Filepath += m_Extension;
		m_Filename += m_Extension;
	}
}

Dialogs::FileDialog::FileDialog(std::function<int(tagOFNA*)> callback, const std::string& dialogTitle) : m_Callback(callback),
m_DialogTitle(dialogTitle),
m_InitialDirectory("")
{
}

void Dialogs::FileDialog::SetInitialDirectory(const std::string& initialDirectory)
{
	m_InitialDirectory = initialDirectory;
}

void Dialogs::FileDialog::Show(ExplorerFlags flags)
{
	OPENFILENAMEA ofn;

	if (!m_InitialDirectory.empty())
		m_Filepath = m_InitialDirectory;

	m_Filepath.resize(MAX_PATH);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = m_Filter.c_str();
	ofn.lpstrFile = m_Filepath.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = m_DialogTitle.c_str();

	if (!m_InitialDirectory.empty())
		ofn.lpstrInitialDir = m_InitialDirectory.c_str();

	ofn.Flags = static_cast<DWORD>(flags);

	m_Succeeded = m_Callback(&ofn);

	if (!m_Succeeded)
		HandleError();
	else
		m_Filepath = m_Filepath.c_str();

	m_Filename.clear();
	for (auto it = m_Filepath.rbegin(); it != m_Filepath.rend() && *it != '\\' && *it != '/'; ++it)
		m_Filename += *it;
	std::reverse(m_Filename.begin(), m_Filename.end());
}

bool Dialogs::FileDialog::HasSucceeded() const
{
	return m_Succeeded;
}

std::string Dialogs::FileDialog::GetSelectedFileName()
{
	return m_Filename;
}

std::string Dialogs::FileDialog::GetSelectedFilePath()
{
	return m_Filepath;
}

std::string Dialogs::FileDialog::GetErrorInfo()
{
	return m_Error;
}

bool Dialogs::FileDialog::IsFileExisting() const
{
	return std::filesystem::exists(m_Filepath);
}

void Dialogs::FileDialog::HandleError()
{
	switch (CommDlgExtendedError())
	{
	case CDERR_DIALOGFAILURE:	m_Error = "CDERR_DIALOGFAILURE";   break;
	case CDERR_FINDRESFAILURE:	m_Error = "CDERR_FINDRESFAILURE";  break;
	case CDERR_INITIALIZATION:	m_Error = "CDERR_INITIALIZATION";  break;
	case CDERR_LOADRESFAILURE:	m_Error = "CDERR_LOADRESFAILURE";  break;
	case CDERR_LOADSTRFAILURE:	m_Error = "CDERR_LOADSTRFAILURE";  break;
	case CDERR_LOCKRESFAILURE:	m_Error = "CDERR_LOCKRESFAILURE";  break;
	case CDERR_MEMALLOCFAILURE: m_Error = "CDERR_MEMALLOCFAILURE"; break;
	case CDERR_MEMLOCKFAILURE:	m_Error = "CDERR_MEMLOCKFAILURE";  break;
	case CDERR_NOHINSTANCE:		m_Error = "CDERR_NOHINSTANCE";     break;
	case CDERR_NOHOOK:			m_Error = "CDERR_NOHOOK";          break;
	case CDERR_NOTEMPLATE:		m_Error = "CDERR_NOTEMPLATE";      break;
	case CDERR_STRUCTSIZE:		m_Error = "CDERR_STRUCTSIZE";      break;
	case FNERR_BUFFERTOOSMALL:	m_Error = "FNERR_BUFFERTOOSMALL";  break;
	case FNERR_INVALIDFILENAME: m_Error = "FNERR_INVALIDFILENAME"; break;
	case FNERR_SUBCLASSFAILURE: m_Error = "FNERR_SUBCLASSFAILURE"; break;
	default:					m_Error = "You cancelled.";
	}
}

Dialogs::OpenFileDialog::OpenFileDialog(const std::string& dialogTitle) : FileDialog(GetOpenFileNameA, dialogTitle)
{
}

void Dialogs::OpenFileDialog::AddFileType(const std::string& label, const std::string& filter)
{
	m_Filter += label + '\0' + filter + '\0';
}
