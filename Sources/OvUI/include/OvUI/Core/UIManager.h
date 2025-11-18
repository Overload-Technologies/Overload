/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <unordered_map>

#include "OvUI/Modules/Canvas.h"
#include "OvUI/Styling/EStyle.h"

namespace OvUI::Core
{
	/**
	* Handle the creation and drawing of the UI
	*/
	class UIManager
	{
	public:
		/**
		* Create the UI manager. Will setup ImGui internally\
		* @param p_glfwWindow
		* @param p_style
		* @param p_glslVersion (Ex: #version 450)
		*/
		UIManager(GLFWwindow* p_glfwWindow, Styling::EStyle p_style = Styling::EStyle::IM_DARK_STYLE, std::string_view p_glslVersion = "#version 450");

		/**
		* Destroy the UI manager. Will handle ImGui destruction internally
		*/
		~UIManager();

		/**
		* Apply a new style to the UI elements
		* @param p_style
		*/
		void ApplyStyle(Styling::EStyle p_style);

		/**
		* Load a font (Returns true on success)
		* @param p_id
		* @param p_path
		* @param p_fontSize
		*/
		bool LoadFont(const std::string& p_id, const std::string& p_path, float p_fontSize);

		/**
		* Unload a font (Returns true on success)
		* @param p_id
		*/
		bool UnloadFont(const std::string& p_id);

		/**
		* Set the given font as the current one (Returns true on success)
		*/
		bool UseFont(const std::string& p_id);

		/**
		* Use the default font (ImGui default font)
		*/
		void UseDefaultFont();

		/**
		* Allow the user to enable/disable .ini generation to save his editor layout
		* @param p_value
		*/
		void EnableEditorLayoutSave(bool p_value);

		/**
		*  Return true if the editor layout save system is on
		*/
		bool IsEditorLayoutSaveEnabled() const;

		/**
		* Defines a filename for the editor layout save file
		*/
		void SetEditorLayoutSaveFilename(const std::string& p_filename);

		/**
		* Defines a frequency (in seconds) for the auto saving system of the editor layout
		* @param p_frequency
		*/
		void SetEditorLayoutAutosaveFrequency(float p_frequency);
		
		/**
		* Returns the current frequency (in seconds) for the auto saving system of the editor layout
		*/
		float GetEditorLayoutAutosaveFrequency(float p_frequeny);

		/**
		* Enable the docking system
		* @param p_value
		*/
		void EnableDocking(bool p_value);

        /**
        * Reset the UI layout to the given configuration file
        * @param p_config
        */
        void ResetLayout(const std::string & p_config) const;

		/**
		* Return true if the docking system is enabled
		*/
		bool IsDockingEnabled() const;

		/**
		* Defines the canvas to use
		* @param p_canvas
		*/
		void SetCanvas(Modules::Canvas& p_canvas);

		/**
		* Stop considering the current canvas (if any)
		*/
		void RemoveCanvas();

		/**
		* Render ImGui current frane
		* @note Should be called once per frame
		*/
		void Render();

	private:
		void PushCurrentFont();
		void PopCurrentFont();

	private:
		bool m_dockingState;
		Modules::Canvas* m_currentCanvas = nullptr;
		std::unordered_map<std::string, ImFont*> m_fonts;
		std::string m_layoutSaveFilename = "imgui.ini";
	};
}