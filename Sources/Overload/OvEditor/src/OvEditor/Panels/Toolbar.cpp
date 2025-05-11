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
	OvUI::Types::Color GetButtonTint(bool p_selected)
	{
		return p_selected ?
			OvUI::Types::Color::Yellow :
			OvUI::Types::Color::White;
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
		using enum Core::EGizmoOperation;
		translate.tint = GetButtonTint(p_operation == TRANSLATE);
		rotate.tint = GetButtonTint(p_operation == ROTATE);
		scale.tint = GetButtonTint(p_operation == SCALE);
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

	m_playButton->lineBreak = false;
	m_pauseButton->lineBreak = false;
	m_stopButton->lineBreak = false;
	m_nextButton->lineBreak = false;
	refreshButton.lineBreak = false;

	m_playButton->ClickedEvent += EDITOR_BIND(StartPlaying);
	m_pauseButton->ClickedEvent += EDITOR_BIND(PauseGame);
	m_stopButton->ClickedEvent += EDITOR_BIND(StopPlaying);
	m_nextButton->ClickedEvent += EDITOR_BIND(NextFrame);
	refreshButton.ClickedEvent += EDITOR_BIND(RefreshScripts);

	EDITOR_EVENT(EditorModeChangedEvent) += [this](Core::EditorActions::EEditorMode p_mode) {
		using enum Core::EditorActions::EEditorMode;
		m_playButton->disabled = !(p_mode == EDIT || p_mode == FRAME_BY_FRAME || p_mode == PAUSE);
		m_pauseButton->disabled = !(p_mode == PLAY);
		m_stopButton->disabled = !(p_mode == PLAY || p_mode == FRAME_BY_FRAME || p_mode == PAUSE);
		m_nextButton->disabled = !(p_mode == PLAY || p_mode == FRAME_BY_FRAME || p_mode == PAUSE);
	};

	EDITOR_EXEC(SetEditorMode(OvEditor::Core::EditorActions::EEditorMode::EDIT));
}

void OvEditor::Panels::Toolbar::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

	OvUI::Panels::PanelWindow::_Draw_Impl();

	ImGui::PopStyleVar();
}