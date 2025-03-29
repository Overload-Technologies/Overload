/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Widgets/Layout/Spacing.h>

#include "OvEditor/Panels/Toolbar.h"
#include "OvEditor/Core/EditorActions.h"

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/TextureManager.h>

OvEditor::Panels::Toolbar::Toolbar
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	std::string iconFolder = ":Textures/Icons/";

	auto& textureManager = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>();

	m_playButton = &CreateWidget<OvUI::Widgets::Buttons::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Play")->id, OvMaths::FVector2{ 20, 20 });
	m_pauseButton = &CreateWidget<OvUI::Widgets::Buttons::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Pause")->id, OvMaths::FVector2{ 20, 20 });
	m_stopButton = &CreateWidget<OvUI::Widgets::Buttons::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Stop")->id, OvMaths::FVector2{ 20, 20 });
	m_nextButton = &CreateWidget<OvUI::Widgets::Buttons::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Next")->id, OvMaths::FVector2{ 20, 20 });

	CreateWidget<OvUI::Widgets::Layout::Spacing>(0).lineBreak = false;
	auto& refreshButton = CreateWidget<OvUI::Widgets::Buttons::ButtonImage>(EDITOR_CONTEXT(editorResources)->GetTexture("Refresh")->id, OvMaths::FVector2{ 20, 20 });

	m_playButton->lineBreak		= false;
	m_pauseButton->lineBreak	= false;
	m_stopButton->lineBreak		= false;
	m_nextButton->lineBreak		= false;
	refreshButton.lineBreak		= false;

	m_playButton->ClickedEvent	+= EDITOR_BIND(StartPlaying);
	m_pauseButton->ClickedEvent	+= EDITOR_BIND(PauseGame);
	m_stopButton->ClickedEvent	+= EDITOR_BIND(StopPlaying);
	m_nextButton->ClickedEvent	+= EDITOR_BIND(NextFrame);
	refreshButton.ClickedEvent	+= EDITOR_BIND(RefreshScripts);

	EDITOR_EVENT(EditorModeChangedEvent) += [this](OvEditor::Core::EditorActions::EEditorMode p_newMode)
	{
		auto enable = [](OvUI::Widgets::Buttons::ButtonImage* p_button, bool p_enable)
		{
			p_button->disabled = !p_enable;
			p_button->tint = p_enable ? OvUI::Types::Color{ 1.0f, 1.0f, 1.0f, 1.0f} : OvUI::Types::Color{1.0f, 1.0f, 1.0f, 0.15f};
		};

		switch (p_newMode)
		{
		case OvEditor::Core::EditorActions::EEditorMode::EDIT:
			enable(m_playButton, true);
			enable(m_pauseButton, false);
			enable(m_stopButton, false);
			enable(m_nextButton, false);
			break;
		case OvEditor::Core::EditorActions::EEditorMode::PLAY:
			enable(m_playButton, false);
			enable(m_pauseButton, true);
			enable(m_stopButton, true);
			enable(m_nextButton, true);
			break;
		case OvEditor::Core::EditorActions::EEditorMode::PAUSE:
			enable(m_playButton, true);
			enable(m_pauseButton, false);
			enable(m_stopButton, true);
			enable(m_nextButton, true);
			break;
		case OvEditor::Core::EditorActions::EEditorMode::FRAME_BY_FRAME:
			enable(m_playButton, true);
			enable(m_pauseButton, false);
			enable(m_stopButton, true);
			enable(m_nextButton, true);
			break;
		}
	};

	EDITOR_EXEC(SetEditorMode(OvEditor::Core::EditorActions::EEditorMode::EDIT));
}

void OvEditor::Panels::Toolbar::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

	OvUI::Panels::PanelWindow::_Draw_Impl();

	ImGui::PopStyleVar();
}