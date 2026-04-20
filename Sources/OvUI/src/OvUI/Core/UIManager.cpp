/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvDebug/Logger.h"
#include <OvDebug/Assertion.h>
#include <OvUI/Core/UIManager.h>
#include <OvUI/Styling/TStyle.h>
#include <OvWindowing/Window.h>

#include <imgui.h>
#include <optional>

namespace
{
	ImGuiStyle GetStyle(OvUI::Styling::EStyle p_style)
	{
		using namespace OvUI::Styling;
		using enum OvUI::Styling::EStyle;

		switch (p_style)
		{
		case IM_CLASSIC_STYLE: return TStyle<IM_CLASSIC_STYLE>::GetStyle();
		case IM_DARK_STYLE: return TStyle<IM_DARK_STYLE>::GetStyle();
		case IM_LIGHT_STYLE: return TStyle<IM_LIGHT_STYLE>::GetStyle();
		case DUNE_DARK: return TStyle<DUNE_DARK>::GetStyle();
		case DEFAULT_DARK: return TStyle<DEFAULT_DARK>::GetStyle();
		case EVEN_DARKER: return TStyle<EVEN_DARKER>::GetStyle();
		}

		return ImGuiStyle();
	}
}

OvUI::Core::UIManager::UIManager(OvWindowing::Window& p_window, Styling::EStyle p_style, std::string_view p_glslVersion) :
	m_window(p_window)
{
	ImGui::CreateContext();

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true; /* Disable moving windows by dragging another thing than the title bar */
	EnableDocking(false);

	ApplyStyle(p_style);
	
	ImGui_ImplGlfw_InitForOpenGL(m_window.GetGlfwWindow(), true);
	ImGui_ImplOpenGL3_Init(p_glslVersion.data());

	m_contentScaleChangedListener = m_window.ContentScaleChangedEvent += [this](float x, float y) {
		if (m_dpiAware)
		{
			SetScale(); // Recompute scale based on content scale
		}
	};
}

OvUI::Core::UIManager::~UIManager()
{
	m_window.ContentScaleChangedEvent -= m_contentScaleChangedListener;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void OvUI::Core::UIManager::ApplyStyle(Styling::EStyle p_style)
{
	m_currentStyle = p_style;
	auto style = GetStyle(p_style);
	style.FontScaleMain = m_scale;
	style.ScaleAllSizes(m_scale);
	ImGui::GetStyle() = style;
}

bool OvUI::Core::UIManager::LoadFont(const std::string& p_id, const std::string & p_path, float p_fontSize)
{
	if (m_fonts.find(p_id) == m_fonts.end())
	{
		auto& io = ImGui::GetIO();
		ImFont* fontInstance = io.Fonts->AddFontFromFileTTF(p_path.c_str(), p_fontSize);

		if (fontInstance)
		{
			m_fonts[p_id] = fontInstance;
			return true;
		}
	}
	
	return false;
}

bool OvUI::Core::UIManager::UnloadFont(const std::string & p_id)
{
	if (m_fonts.find(p_id) != m_fonts.end())
	{
		m_fonts.erase(p_id);
		return true;
	}

	return false;
}

bool OvUI::Core::UIManager::UseFont(const std::string & p_id)
{
	auto foundFont = m_fonts.find(p_id);

	if (foundFont != m_fonts.end())
	{
		ImGui::GetIO().FontDefault = foundFont->second;
		return true;
	}

	return false;
}

void OvUI::Core::UIManager::UseDefaultFont()
{
	ImGui::GetIO().FontDefault = nullptr;
}

void OvUI::Core::UIManager::SetScale(std::optional<float> p_scale)
{
	m_dpiAware = !p_scale.has_value();

	if (m_dpiAware)
	{
		const auto contentScale = m_window.GetContentScale();
		m_scale = std::max(contentScale.first, contentScale.second);
	}
	else
	{
		m_scale = p_scale.value();
	}

	if (m_scale < 1.0f)
	{
		OVLOG_WARNING("UI scale values lower than 1.0f are not supported!");
		m_scale = 1.0f;
	}

	ApplyStyle(m_currentStyle);
}

float OvUI::Core::UIManager::GetScale() const
{
	return m_scale;
}

void OvUI::Core::UIManager::EnableEditorLayoutSave(bool p_value)
{
	if (p_value)
		ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
	else
		ImGui::GetIO().IniFilename = nullptr;
}

bool OvUI::Core::UIManager::IsEditorLayoutSaveEnabled() const
{
	return ImGui::GetIO().IniFilename != nullptr;
}

void OvUI::Core::UIManager::SetEditorLayoutSaveFilename(const std::string & p_filename)
{
	m_layoutSaveFilename = p_filename;
	if (IsEditorLayoutSaveEnabled())
		ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
}

void OvUI::Core::UIManager::SetEditorLayoutAutosaveFrequency(float p_frequency)
{
	ImGui::GetIO().IniSavingRate = p_frequency;
}

float OvUI::Core::UIManager::GetEditorLayoutAutosaveFrequency(float p_frequeny)
{
	return ImGui::GetIO().IniSavingRate;
}

void OvUI::Core::UIManager::EnableMouse(bool p_value)
{
	if (p_value)
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	else
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void OvUI::Core::UIManager::EnableDocking(bool p_value)
{
	m_dockingState = p_value;

	if (p_value)
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	else
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
}

void OvUI::Core::UIManager::ResetLayout(const std::string& p_config) const
{
	ImGui::LoadIniSettingsFromDisk(p_config.c_str());
}

bool OvUI::Core::UIManager::IsDockingEnabled() const
{
	return m_dockingState;
}

void OvUI::Core::UIManager::SetCanvas(Modules::Canvas& p_canvas)
{
	RemoveCanvas();

	m_currentCanvas = &p_canvas;
}

void OvUI::Core::UIManager::RemoveCanvas()
{
	m_currentCanvas = nullptr;
}

void OvUI::Core::UIManager::Render()
{
	if (m_currentCanvas)
	{
		m_currentCanvas->Draw();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

