/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvWindowing/Dialogs/FileDialog.h"

namespace OvWindowing::Dialogs
{
	/**
	* Dialog window that asks the user to save a file to the disk
	*/
	class SaveFileDialog : public FileDialog
	{
	public:
		/**
		* Constructor
		* @param p_dialogTitle
		*/
		SaveFileDialog(const std::string& p_dialogTitle);

		/**
		* Show the file dialog
		* @param p_flags
		*/
		virtual void Show(EExplorerFlags p_flags = EExplorerFlags::DONTADDTORECENT | EExplorerFlags::FILEMUSTEXIST | EExplorerFlags::HIDEREADONLY | EExplorerFlags::NOCHANGEDIR) override;

		/**
		* Define the extension of the saved file
		* @param p_label
		* @param p_extension
		*/
		void DefineExtension(const std::string& p_label, const std::string& p_extension);

	private:
		void AddExtensionToFilePathAndName();

	private:
		std::string m_extension;
	};
}