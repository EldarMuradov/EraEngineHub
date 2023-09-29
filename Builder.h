#pragma once
#include "Include/stdafx.h"
#include <optional>
#include <string>
#include <functional>
#include <commdlg.h>
#include "Project.h"

namespace Dialogs
{
	enum class ExplorerFlags
	{
		READONLY = 0x00000001,
		OVERWRITEPROMPT = 0x00000002,
		HIDEREADONLY = 0x00000004,
		NOCHANGEDIR = 0x00000008,
		SHOWHELP = 0x00000010,
		ENABLEHOOK = 0x00000020,
		ENABLETEMPLATE = 0x00000040,
		ENABLETEMPLATEHANDLE = 0x00000080,
		NOVALIDATE = 0x00000100,
		ALLOWMULTISELECT = 0x00000200,
		EXTENSIONDIFFERENT = 0x00000400,
		PATHMUSTEXIST = 0x00000800,
		FILEMUSTEXIST = 0x00001000,
		CREATEPROMPT = 0x00002000,
		SHAREAWARE = 0x00004000,
		NOREADONLYRETURN = 0x00008000,
		NOTESTFILECREATE = 0x00010000,
		NONETWORKBUTTON = 0x00020000,
		NOLONGNAMES = 0x00040000,	// force no long names for 4.x modules
		EXPLORER = 0x00080000,	// new look commdlg
		NODEREFERENCELINKS = 0x00100000,
		LONGNAMES = 0x00200000,	// force long names for 3.x modules
		ENABLEINCLUDENOTIFY = 0x00400000,	// send include message to callback
		ENABLESIZING = 0x00800000,
		DONTADDTORECENT = 0x02000000,
		FORCESHOWHIDDEN = 0x10000000	// Show All files including System and hidden files
	};

	inline ExplorerFlags operator~ (ExplorerFlags a) { return (ExplorerFlags)~(int)a; }
	inline ExplorerFlags operator| (ExplorerFlags a, ExplorerFlags b) { return (ExplorerFlags)((int)a | (int)b); }
	inline ExplorerFlags operator& (ExplorerFlags a, ExplorerFlags b) { return (ExplorerFlags)((int)a & (int)b); }
	inline ExplorerFlags operator^ (ExplorerFlags a, ExplorerFlags b) { return (ExplorerFlags)((int)a ^ (int)b); }
	inline ExplorerFlags& operator|= (ExplorerFlags& a, ExplorerFlags b) { return (ExplorerFlags&)((int&)a |= (int)b); }
	inline ExplorerFlags& operator&= (ExplorerFlags& a, ExplorerFlags b) { return (ExplorerFlags&)((int&)a &= (int)b); }
	inline ExplorerFlags& operator^= (ExplorerFlags& a, ExplorerFlags b) { return (ExplorerFlags&)((int&)a ^= (int)b); }

	class FileDialog
	{
	public:
		FileDialog(std::function<int(tagOFNA*)> callback, const std::string& dialogTitle);

		void SetInitialDirectory(const std::string& initialDirectory);

		virtual void Show(ExplorerFlags flags = ExplorerFlags::DONTADDTORECENT | ExplorerFlags::FILEMUSTEXIST | ExplorerFlags::HIDEREADONLY | ExplorerFlags::NOCHANGEDIR);

		bool HasSucceeded() const;

		std::string GetSelectedFileName();

		std::string GetSelectedFilePath();

		std::string GetErrorInfo();

		bool IsFileExisting() const;

	private:
		void HandleError();

	protected:
		std::function<int(tagOFNA*)> m_Callback;
		const std::string m_DialogTitle;
		std::string m_InitialDirectory;
		std::string m_Filter;
		std::string m_Error;
		std::string m_Filename;
		std::string m_Filepath;
		bool m_Succeeded;
	};

	class SaveFileDialog : public FileDialog
	{
	public:

		SaveFileDialog(const std::string& dialogTitle);

		virtual void Show(ExplorerFlags flags = ExplorerFlags::DONTADDTORECENT | ExplorerFlags::FILEMUSTEXIST | ExplorerFlags::HIDEREADONLY | ExplorerFlags::NOCHANGEDIR) override;

		void DefineExtension(const std::string& label, const std::string& extension);

	private:
		void AddExtensionToFilePathAndName();

	private:
		std::string m_Extension;
	};


	class OpenFileDialog : public FileDialog
	{
	public:
		OpenFileDialog(const std::string& dialogTitle);

		void AddFileType(const std::string& label, const std::string& filter);
	};
}

namespace OS 
{
	class Builder
	{
	public:
		Builder() = delete;

		static std::optional<std::string> SelectBuildFolder();

		static void Build(Project* project, bool autoRun = false, bool tempFolder = false);
	};
};