/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <functional>


struct tagOFNA;

namespace OvWindowing::Dialogs
{
	/**
	* Some flags that can be passed to FileDialog instances
	*/
	enum class EExplorerFlags
	{
		READONLY                 = 0x00000001,
		OVERWRITEPROMPT          = 0x00000002,
		HIDEREADONLY             = 0x00000004,
		NOCHANGEDIR              = 0x00000008,
		SHOWHELP                 = 0x00000010,
		ENABLEHOOK               = 0x00000020,
		ENABLETEMPLATE           = 0x00000040,
		ENABLETEMPLATEHANDLE     = 0x00000080,
		NOVALIDATE               = 0x00000100,
		ALLOWMULTISELECT         = 0x00000200,
		EXTENSIONDIFFERENT       = 0x00000400,
		PATHMUSTEXIST            = 0x00000800,
		FILEMUSTEXIST            = 0x00001000,
		CREATEPROMPT             = 0x00002000,
		SHAREAWARE               = 0x00004000,
		NOREADONLYRETURN         = 0x00008000,
		NOTESTFILECREATE         = 0x00010000,
		NONETWORKBUTTON          = 0x00020000,
		NOLONGNAMES              = 0x00040000,	// force no long names for 4.x modules
		EXPLORER                 = 0x00080000,	// new look commdlg
		NODEREFERENCELINKS       = 0x00100000,	
		LONGNAMES                = 0x00200000,	// force long names for 3.x modules
		ENABLEINCLUDENOTIFY      = 0x00400000,	// send include message to callback
		ENABLESIZING             = 0x00800000,	
		DONTADDTORECENT          = 0x02000000,	
		FORCESHOWHIDDEN          = 0x10000000	// Show All files including System and hidden files
	};

	inline EExplorerFlags operator~ (EExplorerFlags a) { return (EExplorerFlags)~(int)a; }
	inline EExplorerFlags operator| (EExplorerFlags a, EExplorerFlags b) { return (EExplorerFlags)((int)a | (int)b); }
	inline EExplorerFlags operator& (EExplorerFlags a, EExplorerFlags b) { return (EExplorerFlags)((int)a & (int)b); }
	inline EExplorerFlags operator^ (EExplorerFlags a, EExplorerFlags b) { return (EExplorerFlags)((int)a ^ (int)b); }
	inline EExplorerFlags& operator|= (EExplorerFlags& a, EExplorerFlags b) { return (EExplorerFlags&)((int&)a |= (int)b); }
	inline EExplorerFlags& operator&= (EExplorerFlags& a, EExplorerFlags b) { return (EExplorerFlags&)((int&)a &= (int)b); }
	inline EExplorerFlags& operator^= (EExplorerFlags& a, EExplorerFlags b) { return (EExplorerFlags&)((int&)a ^= (int)b); }

	/**
	* FileDialog is the base class for any dialog window that asks the user to select/save a file from/to the disk
	*/
	class FileDialog
	{
	public:
		/**
		* Constructor
		* @param p_callback
		* @param p_dialogTitle
		*/
		FileDialog(std::function<int(tagOFNA*)> p_callback, const std::string& p_dialogTitle);

		/**
		* Defines the initial directory (Where the FileDialog will open)
		* @param p_initalDirectory
		*/
		void SetInitialDirectory(const std::string& p_initialDirectory);

		/**
		* Show the file dialog
		* @param p_flags
		*/
		virtual void Show(EExplorerFlags p_flags = EExplorerFlags::DONTADDTORECENT | EExplorerFlags::FILEMUSTEXIST | EExplorerFlags::HIDEREADONLY | EExplorerFlags::NOCHANGEDIR);

		/**
		* Returns true if the file action succeeded
		*/
		bool HasSucceeded() const;

		/**
		* Returns the selected file name (Make sur that HasSucceeded() returned true before calling this method)
		*/
		std::string GetSelectedFileName();

		/**
		* Returns the selected file path (Make sur that HasSucceeded() returned true before calling this method)
		*/
		std::string GetSelectedFilePath();

		/**
		* Returns some information about the last error (Make sur that HasSucceeded() returned false before calling this method)
		*/
		std::string GetErrorInfo();

		/**
		* Returns true if the selected file exists
		*/
		bool IsFileExisting() const;

	private:
		void HandleError();

	protected:
		std::function<int(tagOFNA*)> m_callback;
		const std::string m_dialogTitle;
		std::string m_initialDirectory;
		std::string m_filter;
		std::string m_error;
		std::string m_filename;
		std::string m_filepath;
		bool m_succeeded;
		bool m_isSaveDialog = false;
	};
}