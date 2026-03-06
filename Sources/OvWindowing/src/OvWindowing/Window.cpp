/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvWindowing/Window.h"

// #define STB_IMAGE_IMPLEMENTATION

#include <stb_image/stb_image.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#include <GLFW/glfw3.h>

#include <stdexcept>

std::unordered_map<GLFWwindow*, OvWindowing::Window*> OvWindowing::Window::__WINDOWS_MAP;

OvWindowing::Window::Window(const Context::Device& p_device, const Settings::WindowSettings& p_windowSettings) :
	m_device(p_device),
	m_title(p_windowSettings.title),
	m_size{ p_windowSettings.width, p_windowSettings.height },
	m_position{ p_windowSettings.x, p_windowSettings.y },
	m_minimumSize { p_windowSettings.minimumWidth, p_windowSettings.minimumHeight },
	m_maximumSize { p_windowSettings.maximumWidth, p_windowSettings.maximumHeight },
	m_fullscreen(p_windowSettings.fullscreen),
	m_refreshRate(p_windowSettings.refreshRate),
	m_cursorMode(p_windowSettings.cursorMode),
	m_cursorShape(p_windowSettings.cursorShape)
{
	/* Window creation */
	CreateGlfwWindow(p_windowSettings);

	/* Window settings */
	SetCursorMode(p_windowSettings.cursorMode);
	SetCursorShape(p_windowSettings.cursorShape);

	/* Callback binding */
	BindKeyCallback();
	BindMouseCallback();
	BindScrollCallback();
	BindIconifyCallback();
	BindCloseCallback();
	BindResizeCallback();
	BindCursorMoveCallback();
	BindFramebufferResizeCallback();
	BindMoveCallback();
	BindFocusCallback();

	/* Event listening */
	ResizeEvent.AddListener(std::bind(&Window::OnResize, this, std::placeholders::_1, std::placeholders::_2));
	MoveEvent.AddListener(std::bind(&Window::OnMove, this, std::placeholders::_1, std::placeholders::_2));
}

OvWindowing::Window::~Window()
{
	glfwDestroyWindow(m_glfwWindow);
}

void OvWindowing::Window::SetIcon(const std::string & p_filePath)
{
	GLFWimage images[1];
	images[0].pixels = stbi_load(p_filePath.c_str(), &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(m_glfwWindow, 1, images);
}

void OvWindowing::Window::SetIconFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height)
{
	GLFWimage images[1];
	images[0].pixels = p_data;
	images[0].height = p_width;
	images[0].width = p_height;
	glfwSetWindowIcon(m_glfwWindow, 1, images);
}

OvWindowing::Window* OvWindowing::Window::FindInstance(GLFWwindow* p_glfwWindow)
{
	return __WINDOWS_MAP.find(p_glfwWindow) != __WINDOWS_MAP.end() ? __WINDOWS_MAP[p_glfwWindow] : nullptr;
}

void OvWindowing::Window::SetSize(uint16_t p_width, uint16_t p_height)
{
	glfwSetWindowSize(m_glfwWindow, static_cast<int>(p_width), static_cast<int>(p_height));
}

void OvWindowing::Window::SetMinimumSize(int16_t p_minimumWidth, int16_t p_minimumHeight)
{
	m_minimumSize.first = p_minimumWidth;
	m_minimumSize.second = p_minimumHeight;

	UpdateSizeLimit();
}

void OvWindowing::Window::SetMaximumSize(int16_t p_maximumWidth, int16_t p_maximumHeight)
{
	m_maximumSize.first = p_maximumWidth;
	m_maximumSize.second = p_maximumHeight;

	UpdateSizeLimit();
}

void OvWindowing::Window::SetPosition(int16_t p_x, int16_t p_y)
{
	glfwSetWindowPos(m_glfwWindow, static_cast<int>(p_x), static_cast<int>(p_y));
}

void OvWindowing::Window::Minimize() const
{
	glfwIconifyWindow(m_glfwWindow);
}

void OvWindowing::Window::Maximize() const
{
	glfwMaximizeWindow(m_glfwWindow);
}

void OvWindowing::Window::Restore() const
{
	glfwRestoreWindow(m_glfwWindow);
}

void OvWindowing::Window::Hide() const
{
	glfwHideWindow(m_glfwWindow);
}

void OvWindowing::Window::Show() const
{
	glfwShowWindow(m_glfwWindow);
}

void OvWindowing::Window::Focus() const
{
	glfwFocusWindow(m_glfwWindow);
}

void OvWindowing::Window::SetShouldClose(bool p_value) const
{
	glfwSetWindowShouldClose(m_glfwWindow, p_value);
}

bool OvWindowing::Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_glfwWindow);
}

void OvWindowing::Window::SetFullscreen(bool p_value)
{
	if (p_value)
		m_fullscreen = true;

	glfwSetWindowMonitor
	(
		m_glfwWindow,
		p_value ? glfwGetPrimaryMonitor() : nullptr,
		static_cast<int>(m_position.first),
		static_cast<int>(m_position.second),
		static_cast<int>(m_size.first),
		static_cast<int>(m_size.second),
		m_refreshRate
	);

	if (!p_value)
		m_fullscreen = false;

}

void OvWindowing::Window::ToggleFullscreen()
{
	SetFullscreen(!m_fullscreen);
}

bool OvWindowing::Window::IsFullscreen() const
{
	return m_fullscreen;
}

bool OvWindowing::Window::IsHidden() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_FALSE;
}

bool OvWindowing::Window::IsVisible() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_TRUE;
}

bool OvWindowing::Window::IsMaximized() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
}

bool OvWindowing::Window::IsMinimized() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_FALSE;
}

bool OvWindowing::Window::IsFocused() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == GLFW_TRUE;
}

bool OvWindowing::Window::IsResizable() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_RESIZABLE) == GLFW_TRUE;
}

bool OvWindowing::Window::IsDecorated() const
{
	return glfwGetWindowAttrib(m_glfwWindow, GLFW_DECORATED) == GLFW_TRUE;;
}

void OvWindowing::Window::MakeCurrentContext() const
{
	glfwMakeContextCurrent(m_glfwWindow);
}

void OvWindowing::Window::SwapBuffers()
{
	if (m_cursorShape.has_value())
	{
		glfwSetCursor(m_glfwWindow, m_device.GetCursorInstance(m_cursorShape.value()));
		m_cursorShape.reset();
	}

	glfwSwapBuffers(m_glfwWindow);
}

void OvWindowing::Window::SetCursorMode(Cursor::ECursorMode p_cursorMode)
{
	m_cursorMode = p_cursorMode;

	// Workaround to avoid mouse issues with RDP sessions on Windows. (https://github.com/glfw/glfw/issues/2463)
	// This effectively disable the DISABLED cursor mode when in a remote session.
	// This is far from ideal, but at least it prevents the mouse from going crazy,
	// which keeps the application usable.
#if defined(_WIN32)
	if (p_cursorMode == Cursor::ECursorMode::DISABLED && GetSystemMetrics(SM_REMOTESESSION))
	{
		return;
	}
#endif

	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, static_cast<int>(p_cursorMode));
}

void OvWindowing::Window::SetCursorShape(Cursor::ECursorShape p_cursorShape)
{
	m_cursorShape = p_cursorShape;
}

void OvWindowing::Window::SetCursorPosition(int16_t p_x, int16_t p_y)
{
	glfwSetCursorPos(m_glfwWindow, static_cast<double>(p_x), static_cast<double>(p_y));
}

void OvWindowing::Window::SetTitle(const std::string& p_title)
{
	m_title = p_title;
	glfwSetWindowTitle(m_glfwWindow, p_title.c_str());
}

void OvWindowing::Window::SetRefreshRate(int32_t p_refreshRate)
{
	m_refreshRate = p_refreshRate;
}

std::string OvWindowing::Window::GetTitle() const
{
	return m_title;
}

std::pair<uint16_t, uint16_t> OvWindowing::Window::GetSize() const
{
	int width, height;
	glfwGetWindowSize(m_glfwWindow, &width, &height);
	return std::make_pair(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
}

std::pair<int16_t, int16_t> OvWindowing::Window::GetMinimumSize() const
{
	return m_minimumSize;
}

std::pair<int16_t, int16_t> OvWindowing::Window::GetMaximumSize() const
{
	return m_maximumSize;
}

std::pair<int16_t, int16_t> OvWindowing::Window::GetPosition() const
{
	int x, y;
	glfwGetWindowPos(m_glfwWindow, &x, &y);
	return std::make_pair(static_cast<int16_t>(x), static_cast<int16_t>(y));
}

std::pair<uint16_t, uint16_t> OvWindowing::Window::GetFramebufferSize() const
{
	int width, height;
	glfwGetFramebufferSize(m_glfwWindow, &width, &height);
	return std::make_pair(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
}

OvWindowing::Cursor::ECursorMode OvWindowing::Window::GetCursorMode() const
{
	return m_cursorMode;
}

int32_t OvWindowing::Window::GetRefreshRate() const
{
	return m_refreshRate;
}

GLFWwindow* OvWindowing::Window::GetGlfwWindow() const
{
	return m_glfwWindow;
}

void OvWindowing::Window::CreateGlfwWindow(const Settings::WindowSettings& p_windowSettings)
{
	GLFWmonitor* selectedMonitor = nullptr;

	if (m_fullscreen)
		selectedMonitor = glfwGetPrimaryMonitor();

	glfwWindowHint(GLFW_RESIZABLE,		p_windowSettings.resizable);
	glfwWindowHint(GLFW_DECORATED,		p_windowSettings.decorated);
	glfwWindowHint(GLFW_FOCUSED,		p_windowSettings.focused);
	glfwWindowHint(GLFW_MAXIMIZED,		p_windowSettings.maximized);
	glfwWindowHint(GLFW_FLOATING,		p_windowSettings.floating);
	glfwWindowHint(GLFW_VISIBLE,		p_windowSettings.visible);
	glfwWindowHint(GLFW_AUTO_ICONIFY,	p_windowSettings.autoIconify);
	glfwWindowHint(GLFW_REFRESH_RATE,	p_windowSettings.refreshRate);
	glfwWindowHint(GLFW_SAMPLES,		p_windowSettings.samples);

	m_glfwWindow = glfwCreateWindow(static_cast<int>(m_size.first), static_cast<int>(m_size.second), m_title.c_str(), selectedMonitor, nullptr);

	if (!m_glfwWindow)
	{
		throw std::runtime_error("Failed to create GLFW window"); // TODO: Replace with OvDebug assertion
	}
	else
	{
		UpdateSizeLimit();
		SetPosition(m_position.first, m_position.second);
		__WINDOWS_MAP[m_glfwWindow] = this;
	}
}

void OvWindowing::Window::BindKeyCallback() const
{
	auto keyCallback = [](GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			if (p_action == GLFW_PRESS)
				windowInstance->KeyPressedEvent.Invoke(p_key);

			if (p_action == GLFW_RELEASE)
				windowInstance->KeyReleasedEvent.Invoke(p_key);
		}
	};

	glfwSetKeyCallback(m_glfwWindow, keyCallback);
}

void OvWindowing::Window::BindMouseCallback() const
{
	auto mouseCallback = [](GLFWwindow* p_window, int p_button, int p_action, int p_mods)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			if (p_action == GLFW_PRESS)
				windowInstance->MouseButtonPressedEvent.Invoke(p_button);

			if (p_action == GLFW_RELEASE)
				windowInstance->MouseButtonReleasedEvent.Invoke(p_button);
		}
	};

	glfwSetMouseButtonCallback(m_glfwWindow, mouseCallback);
}

void OvWindowing::Window::BindScrollCallback() const
{
	auto scrollCallback = [](GLFWwindow* p_window, double p_xOffset, double p_yOffset)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			windowInstance->MouseScrollEvent.Invoke(p_xOffset, p_yOffset);
		}
	};

	glfwSetScrollCallback(m_glfwWindow, scrollCallback);
}

void OvWindowing::Window::BindResizeCallback() const
{
	auto resizeCallback = [](GLFWwindow* p_window, int p_width, int p_height)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			windowInstance->ResizeEvent.Invoke(static_cast<uint16_t>(p_width), static_cast<uint16_t>(p_height));
		}
	};

	glfwSetWindowSizeCallback(m_glfwWindow, resizeCallback);
}

void OvWindowing::Window::BindFramebufferResizeCallback() const
{
	auto framebufferResizeCallback = [](GLFWwindow* p_window, int p_width, int p_height)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			windowInstance->FramebufferResizeEvent.Invoke(static_cast<uint16_t>(p_width), static_cast<uint16_t>(p_height));
		}
	};

	glfwSetFramebufferSizeCallback(m_glfwWindow, framebufferResizeCallback);
}

void OvWindowing::Window::BindCursorMoveCallback() const
{
	auto cursorMoveCallback = [](GLFWwindow* p_window, double p_x, double p_y)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			windowInstance->CursorMoveEvent.Invoke(static_cast<int16_t>(p_x), static_cast<int16_t>(p_y));
		}
	};

	glfwSetCursorPosCallback(m_glfwWindow, cursorMoveCallback);
}

void OvWindowing::Window::BindMoveCallback() const
{
	auto moveCallback = [](GLFWwindow* p_window, int p_x, int p_y)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			windowInstance->MoveEvent.Invoke(static_cast<int16_t>(p_x), static_cast<int16_t>(p_y));
		}
	};

	glfwSetWindowPosCallback(m_glfwWindow, moveCallback);
}

void OvWindowing::Window::BindIconifyCallback() const
{
	auto iconifyCallback = [](GLFWwindow* p_window, int p_iconified)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			if (p_iconified == GLFW_TRUE)
				windowInstance->MinimizeEvent.Invoke();

			if (p_iconified == GLFW_FALSE)
				windowInstance->MaximizeEvent.Invoke();
		}
	};

	glfwSetWindowIconifyCallback(m_glfwWindow, iconifyCallback);
}

void OvWindowing::Window::BindFocusCallback() const
{
	auto focusCallback = [](GLFWwindow* p_window, int p_focused)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			if (p_focused == GLFW_TRUE)
				windowInstance->GainFocusEvent.Invoke();

			if (p_focused == GLFW_FALSE)
				windowInstance->LostFocusEvent.Invoke();
		}
	};

	glfwSetWindowFocusCallback(m_glfwWindow, focusCallback);
}

void OvWindowing::Window::BindCloseCallback() const
{
	auto closeCallback = [](GLFWwindow* p_window)
	{
		Window* windowInstance = FindInstance(p_window);

		if (windowInstance)
		{
			windowInstance->CloseEvent.Invoke();
		}
	};

	glfwSetWindowCloseCallback(m_glfwWindow, closeCallback);
}

void OvWindowing::Window::OnResize(uint16_t p_width, uint16_t p_height)
{
	m_size.first = p_width;
	m_size.second = p_height;
}

void OvWindowing::Window::OnMove(int16_t p_x, int16_t p_y)
{
	if (!m_fullscreen)
	{
		m_position.first = p_x;
		m_position.second = p_y;
	}
}

void OvWindowing::Window::UpdateSizeLimit() const
{
	glfwSetWindowSizeLimits
	(
		m_glfwWindow,
		static_cast<int>(m_minimumSize.first),
		static_cast<int>(m_minimumSize.second),
		static_cast<int>(m_maximumSize.first),
		static_cast<int>(m_maximumSize.second)
	);
}
