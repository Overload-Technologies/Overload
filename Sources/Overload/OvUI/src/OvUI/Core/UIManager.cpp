/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Core/UIManager.h>
#include <OvUI/Styling/TStyle.h>

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

#include <filesystem>

#include <OvTools/Utils/SystemCalls.h>

OvUI::Core::UIManager::UIManager(GLFWwindow* p_glfwWindow, Styling::EStyle p_style, std::string_view p_glslVersion) :
m_defaultLayout{ std::filesystem::path{} / "Config" / "layout.ini" },
m_layoutsPath(std::filesystem::path { OvTools::Utils::SystemCalls::GetPathToAppdata() } / "OverloadTech" / "OvEditor" / "Layouts")
{
	ImGui::CreateContext();

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true; /* Disable moving windows by dragging another thing than the title bar */
	EnableDocking(false);

	ApplyStyle(p_style);
	
	ImGui_ImplGlfw_InitForOpenGL(p_glfwWindow, true);
	ImGui_ImplOpenGL3_Init(p_glslVersion.data());

	std::filesystem::create_directories(m_layoutsPath);
}

OvUI::Core::UIManager::~UIManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void OvUI::Core::UIManager::ApplyStyle(Styling::EStyle p_style)
{
	ImGui::GetStyle() = GetStyle(p_style);
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

void OvUI::Core::UIManager::SetEditorLayoutSaveFilename(const std::filesystem::path& p_filePath)
{
	m_layoutSaveFilename = p_filePath.string();

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

void OvUI::Core::UIManager::EnableDocking(bool p_value)
{
	m_dockingState = p_value;

	if (p_value)
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	else
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
}

void OvUI::Core::UIManager::ResetToDefaultLayout() const
{
	ImGui::LoadIniSettingsFromDisk(m_defaultLayout.string().c_str());
}

void OvUI::Core::UIManager::ResetLayout(const std::string& p_config) const
{
	ImGui::LoadIniSettingsFromDisk(p_config.c_str());
}

void OvUI::Core::UIManager::SaveLayout(const std::filesystem::path& p_filePath)
{
	SetEditorLayoutSaveFilename(p_filePath);

	ImGui::SaveIniSettingsToDisk(m_layoutSaveFilename.c_str());
}

void OvUI::Core::UIManager::SaveCurrentLayout()
{
	if (!std::filesystem::exists(m_layoutSaveFilename))
	{
		auto path = m_layoutsPath / "layout.ini";
		m_layoutSaveFilename = path.string();

		SetEditorLayoutSaveFilename(path);
	}
	ImGui::SaveIniSettingsToDisk(m_layoutSaveFilename.c_str());
}

void OvUI::Core::UIManager::SetIniLayout(const std::string& p_fileName)
{
	auto iniLayoutPath = m_layoutsPath / (p_fileName + ".ini");

	if(std::filesystem::exists(iniLayoutPath))
		SetLayout(iniLayoutPath);
}

void OvUI::Core::UIManager::SetLayout(const std::filesystem::path& p_filePath)
{
	SetEditorLayoutSaveFilename(p_filePath);

	ImGui::LoadIniSettingsFromDisk(p_filePath.string().c_str());
}

void OvUI::Core::UIManager::DeleteLayout(const std::filesystem::path& p_filePath)
{
	std::filesystem::remove(p_filePath);
}

void OvUI::Core::UIManager::RenameLayout(const std::filesystem::path& p_filePath, const std::filesystem::path& p_newFilePath)
{
	std::filesystem::rename(p_filePath, p_newFilePath);

	if (m_layoutSaveFilename == p_filePath)
	{
		SetEditorLayoutSaveFilename(p_newFilePath);
	}
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

const std::filesystem::path& OvUI::Core::UIManager::GetLayoutsPath() const
{
	return m_layoutsPath;
}

void OvUI::Core::UIManager::PushCurrentFont()
{

}

void OvUI::Core::UIManager::PopCurrentFont()
{

}
