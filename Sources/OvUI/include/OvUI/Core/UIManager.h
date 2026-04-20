/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvTools/Eventing/Event.h"
#include <optional>
#include <string>
#include <unordered_map>

#include <OvUI/Modules/Canvas.h>
#include <OvUI/Styling/EStyle.h>

namespace OvWindowing
{
	class Window;
}

namespace OvUI::Core
{
	/**
	* Handle the creation and drawing of the UI
	*/
	class UIManager
	{
	public:
		/**
		* Create the UI manager. Will setup ImGui internally
		* @param p_window
		* @param p_style
		* @param p_glslVersion (Ex: #version 450)
		*/
		UIManager(OvWindowing::Window& p_window, Styling::EStyle p_style = Styling::EStyle::IM_DARK_STYLE, std::string_view p_glslVersion = "#version 450");

		/**
		* Destroy the UI manager. Will handle ImGui destruction internally
		*/
		virtual ~UIManager();

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
		* Set the UI scale
		* @param p_scale
		*/
		void SetScale(std::optional<float> p_scale = std::nullopt);

		/**
		* Returns the scale of the UI
		*/
		float GetScale() const;

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
		* Enable or disable mouse input in ImGui.
		* Typically disabled when the cursor is locked during gameplay.
		* @param p_value
		*/
		void EnableMouse(bool p_value);

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
		OvWindowing::Window& m_window;
		OvTools::Eventing::ListenerID m_contentScaleChangedListener;
		bool m_dockingState;
		Styling::EStyle m_currentStyle;
		float m_scale;
		bool m_dpiAware;
		bool m_refreshStyle = false;
		Modules::Canvas* m_currentCanvas = nullptr;
		std::unordered_map<std::string, ImFont*> m_fonts;
		std::string m_layoutSaveFilename = "imgui.ini";
	};
}
