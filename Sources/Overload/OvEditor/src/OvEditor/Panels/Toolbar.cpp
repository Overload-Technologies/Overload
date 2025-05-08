/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/TextureManager.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/GizmoBehaviour.h>
#include <OvEditor/Panels/Toolbar.h>

#include <OvUI/Widgets/Layout/Spacing.h>

namespace
{
	void SetButtonState(OvUI::Widgets::Buttons::ButtonImage* p_button, bool p_enable)
	{
		p_button->disabled = !p_enable;
		p_button->tint = p_enable ? OvUI::Types::Color{ 1.0f, 1.0f, 1.0f, 1.0f } : OvUI::Types::Color{ 1.0f, 1.0f, 1.0f, 0.15f };
	}
}

OvEditor::Panels::Toolbar::Toolbar
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	using namespace OvUI::Widgets;
	using namespace OvUI::Widgets::Buttons;

	const auto iconSize = OvMaths::FVector2{ 20, 20 };
	auto& textureManager = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>();
	auto& editorResources = EDITOR_CONTEXT(editorResources);

	auto& translate = CreateWidget<ButtonImage>(editorResources->GetTexture("Move")->GetTexture().GetID(), iconSize);
	translate.lineBreak = false;
	translate.ClickedEvent += []() { EDITOR_EXEC(SetGizmoOperation(OvEditor::Core::EGizmoOperation::TRANSLATE)); };

	auto& rotate = CreateWidget<ButtonImage>(editorResources->GetTexture("Rotate")->GetTexture().GetID(), iconSize);
	rotate.lineBreak = false;
	rotate.ClickedEvent += []() { EDITOR_EXEC(SetGizmoOperation(OvEditor::Core::EGizmoOperation::ROTATE)); };

	auto& scale = CreateWidget<ButtonImage>(editorResources->GetTexture("Scale")->GetTexture().GetID(), iconSize);
	scale.lineBreak = false;
	scale.ClickedEvent += []() { EDITOR_EXEC(SetGizmoOperation(OvEditor::Core::EGizmoOperation::SCALE)); };

	auto updateGizmoOperation = [&translate, &rotate, &scale](Core::EGizmoOperation p_operation) {
		switch (p_operation)
		{
		case Core::EGizmoOperation::TRANSLATE:
			SetButtonState(&translate, true);
			SetButtonState(&rotate, false);
			SetButtonState(&scale, false);
			break;
		case Core::EGizmoOperation::ROTATE:
			SetButtonState(&translate, false);
			SetButtonState(&rotate, true);
			SetButtonState(&scale, false);
			break;
		case Core::EGizmoOperation::SCALE:
			SetButtonState(&translate, false);
			SetButtonState(&rotate, false);
			SetButtonState(&scale, true);
			break;
		}
	};

	updateGizmoOperation(EDITOR_EXEC(GetGizmoOperation()));

	EDITOR_EVENT(EditorOperationChanged) += updateGizmoOperation;

	CreateWidget<Layout::Spacing>().lineBreak = false;

	m_playButton = &CreateWidget<ButtonImage>(editorResources->GetTexture("Play")->GetTexture().GetID(), iconSize);
	m_pauseButton = &CreateWidget<ButtonImage>(editorResources->GetTexture("Pause")->GetTexture().GetID(), iconSize);
	m_stopButton = &CreateWidget<ButtonImage>(editorResources->GetTexture("Stop")->GetTexture().GetID(), iconSize);
	m_nextButton = &CreateWidget<ButtonImage>(editorResources->GetTexture("Next")->GetTexture().GetID(), iconSize);

	CreateWidget<Layout::Spacing>(0).lineBreak = false;
	auto& refreshButton = CreateWidget<ButtonImage>(editorResources->GetTexture("Refresh")->GetTexture().GetID(), iconSize);

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

	EDITOR_EVENT(EditorModeChangedEvent) += [this](Core::EditorActions::EEditorMode p_newMode)
	{
		switch (p_newMode)
		{
		case Core::EditorActions::EEditorMode::EDIT:
			SetButtonState(m_playButton, true);
			SetButtonState(m_pauseButton, false);
			SetButtonState(m_stopButton, false);
			SetButtonState(m_nextButton, false);
			break;
		case Core::EditorActions::EEditorMode::PLAY:
			SetButtonState(m_playButton, false);
			SetButtonState(m_pauseButton, true);
			SetButtonState(m_stopButton, true);
			SetButtonState(m_nextButton, true);
			break;
		case Core::EditorActions::EEditorMode::PAUSE:
			SetButtonState(m_playButton, true);
			SetButtonState(m_pauseButton, false);
			SetButtonState(m_stopButton, true);
			SetButtonState(m_nextButton, true);
			break;
		case Core::EditorActions::EEditorMode::FRAME_BY_FRAME:
			SetButtonState(m_playButton, true);
			SetButtonState(m_pauseButton, false);
			SetButtonState(m_stopButton, true);
			SetButtonState(m_nextButton, true);
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