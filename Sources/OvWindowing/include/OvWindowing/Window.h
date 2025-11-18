/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <optional>

#include <OvWindowing/Context/Device.h>
#include <OvWindowing/Cursor/ECursorShape.h>
#include <OvWindowing/Cursor/ECursorMode.h>
#include <OvWindowing/Settings/WindowSettings.h>

namespace OvWindowing
{
	/**
	* A simple OS-based window.
	* It needs a Device (GLFW) to work
	*/
	class Window
	{
	public:
		/* Inputs events */
		OvTools::Eventing::Event<int> KeyPressedEvent;
		OvTools::Eventing::Event<int> KeyReleasedEvent;
		OvTools::Eventing::Event<int> MouseButtonPressedEvent;
		OvTools::Eventing::Event<int> MouseButtonReleasedEvent;
		OvTools::Eventing::Event<double, double> MouseScrollEvent;

		/* Window events */
		OvTools::Eventing::Event<uint16_t, uint16_t> ResizeEvent;
		OvTools::Eventing::Event<uint16_t, uint16_t> FramebufferResizeEvent;
		OvTools::Eventing::Event<int16_t, int16_t> MoveEvent;
		OvTools::Eventing::Event<int16_t, int16_t> CursorMoveEvent;
		OvTools::Eventing::Event<> MinimizeEvent;
		OvTools::Eventing::Event<> MaximizeEvent;
		OvTools::Eventing::Event<> GainFocusEvent;
		OvTools::Eventing::Event<> LostFocusEvent;
		OvTools::Eventing::Event<> CloseEvent;

		/**
		* Create the window
		* @param p_device
		* @param p_windowSettings
		*/
		Window(const Context::Device& p_device, const Settings::WindowSettings& p_windowSettings);

		/**
		* Destructor of the window, responsible of the GLFW window memory free
		*/
		~Window();

		/**
		* Set Icon
		* @param p_filePath
		*/
		void SetIcon(const std::string& p_filePath);

		/**
		* Set Icon from memory
		* @param p_data
		* @param p_width
		* @param p_height
		*/
		void SetIconFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height);

		/**
		* Find an instance of window with a given GLFWwindow
		* @param p_glfwWindow
		*/
		static Window* FindInstance(GLFWwindow* p_glfwWindow);

		/**
		* Resize the window
		* @param p_width
		* @param p_height
		*/
		void SetSize(uint16_t p_width, uint16_t p_height);

		/**
		* Defines a minimum size for the window
		* @param p_minimumWidth
		* @param p_minimumHeight
		* @note -1 (WindowSettings::DontCare) value means no limitation
		*/
		void SetMinimumSize(int16_t p_minimumWidth, int16_t p_minimumHeight);

		/**
		* Defines a maximum size for the window
		* @param p_maximumWidth
		* @param p_maximumHeight
		* @note -1 (WindowSettings::DontCare) value means no limitation
		*/
		void SetMaximumSize(int16_t p_maximumWidth, int16_t p_maximumHeight);

		/**
		* Define a position for the window
		* @param p_x
		* @param p_y
		*/
		void SetPosition(int16_t p_x, int16_t p_y);

		/**
		* Minimize the window
		*/
		void Minimize() const;

		/**
		* Maximize the window
		*/
		void Maximize() const;

		/**
		* Restore the window
		*/
		void Restore() const;

		/**
		* Hides the specified window if it was previously visible
		*/
		void Hide() const;

		/**
		* Show the specified window if it was previously hidden
		*/
		void Show() const;

		/**
		* Focus the window
		*/
		void Focus() const;

		/**
		* Set the should close flag of the window to true
		* @param p_value
		*/
		void SetShouldClose(bool p_value) const;

		/**
		* Return true if the window should close
		*/
		bool ShouldClose() const;

		/**
		* Set the window in fullscreen or windowed mode
		* @param p_value (True for fullscreen mode, false for windowed)
		*/
		void SetFullscreen(bool p_value);

		/**
		* Switch the window to fullscreen or windowed mode depending
		* on the current state
		*/
		void ToggleFullscreen();

		/**
		* Return true if the window is fullscreen
		*/
		bool IsFullscreen() const;

		/**
		* Return true if the window is hidden
		*/
		bool IsHidden() const;

		/**
		* Return true if the window is visible
		*/
		bool IsVisible() const;

		/**
		* Return true if the windows is maximized
		*/
		bool IsMaximized() const;

		/**
		* Return true if the windows is minimized
		*/
		bool IsMinimized() const;

		/**
		* Return true if the windows is focused
		*/
		bool IsFocused() const;

		/**
		* Return true if the windows is resizable
		*/
		bool IsResizable() const;

		/**
		* Return true if the windows is decorated
		*/
		bool IsDecorated() const;

		/**
		* Define the window as the current context
		*/
		void MakeCurrentContext() const;

		/**
		* Handle the buffer swapping with the current window.
		* @note Also responsible for updating the cursor, so it's done last and has the final word
		*/
		void SwapBuffers();

		/**
		* Define a mode for the mouse cursor
		* @param p_cursorMode
		*/
		void SetCursorMode(Cursor::ECursorMode p_cursorMode);

		/**
		* Define a shape to apply to the current cursor.
		* @param p_cursorShape
		* @note the cursor will only update on the next SwapBuffers call, this is so that OvWindowing has the final word.
		* @note when using ImGui, this needs to be called every frame to update the cursor shape
		*/
		void SetCursorShape(Cursor::ECursorShape p_cursorShape);

		/**
		* Move the cursor to the given position
		*/
		void SetCursorPosition(int16_t p_x, int16_t p_y);

		/**
		* Define a title for the window
		* @param p_title
		*/
		void SetTitle(const std::string& p_title);

		/**
		* Defines a refresh rate (Use WindowSettings::DontCare to use the highest available refresh rate)
		* @param p_refreshRate
		* @note You need to switch to fullscreen mode to apply this effect (Or leave fullscreen and re-apply)
		*/
		void SetRefreshRate(int32_t p_refreshRate);

		/**
		* Return the title of the window
		*/
		std::string GetTitle() const;

		/**
		* Return the current size of the window
		*/
		std::pair<uint16_t, uint16_t> GetSize() const;

		/**
		* Return the current minimum size of the window
		* @note -1 (WindowSettings::DontCare) values means no limitation
		*/
		std::pair<int16_t, int16_t> GetMinimumSize() const;

		/**
		* Return the current maximum size of the window
		* @note -1 (WindowSettings::DontCare) values means no limitation
		*/
		std::pair<int16_t, int16_t> GetMaximumSize() const;

		/**
		* Return the current position of the window
		*/
		std::pair<int16_t, int16_t> GetPosition() const;

		/**
		* Return the framebuffer size (Viewport size)
		*/
		std::pair<uint16_t, uint16_t> GetFramebufferSize() const;

		/**
		* Return the current cursor mode
		*/
		Cursor::ECursorMode GetCursorMode() const;

		/**
		* Return the current refresh rate (Only applied to the fullscreen mode).
		* If the value is -1 (WindowSettings::DontCare) the highest refresh rate will be used
		*/
		int32_t GetRefreshRate() const;

		/**
		* Return GLFW window
		*/
		GLFWwindow* GetGlfwWindow() const;

	private:
		void CreateGlfwWindow(const Settings::WindowSettings& p_windowSettings);

		/* Callbacks binding */
		void BindKeyCallback() const;
		void BindMouseCallback() const;
		void BindScrollCallback() const;
		void BindResizeCallback() const;
		void BindFramebufferResizeCallback() const;
		void BindCursorMoveCallback() const;
		void BindMoveCallback() const;
		void BindIconifyCallback() const;
		void BindFocusCallback() const;
		void BindCloseCallback() const;

		/* Event listeners */
		void OnResize(uint16_t p_width, uint16_t p_height);
		void OnMove(int16_t p_x, int16_t p_y);

		/* Internal helpers */
		void UpdateSizeLimit() const;

	private:
		/* This map is used by callbacks to find a "Window" instance out of a "GLFWwindow" instnace*/
		static std::unordered_map<GLFWwindow*, Window*> __WINDOWS_MAP;

		const Context::Device& m_device;
		GLFWwindow* m_glfwWindow;

		/* Window settings */
		std::string m_title;
		std::pair<uint16_t, uint16_t> m_size;
		std::pair<int16_t, int16_t> m_minimumSize;
		std::pair<int16_t, int16_t> m_maximumSize;
		std::pair<int16_t, int16_t> m_position;
		bool m_fullscreen;
		int32_t m_refreshRate;
		Cursor::ECursorMode m_cursorMode;
		std::optional<Cursor::ECursorShape> m_cursorShape;
	};
}
