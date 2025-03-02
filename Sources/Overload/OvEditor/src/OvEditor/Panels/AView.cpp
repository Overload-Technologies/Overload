/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Panels/AView.h>
#include <OvEditor/Settings/EditorSettings.h>

OvEditor::Panels::AView::AView
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	OvCore::Rendering::FramebufferUtil::SetupFramebuffer(
		m_framebuffer,
		static_cast<uint32_t>(GetSize().x),
		static_cast<uint32_t>(GetSize().y),
		true, true, false
	);

	const auto tex = m_framebuffer.GetAttachment<OvRendering::HAL::Texture>(OvRendering::Settings::EFramebufferAttachment::COLOR);
	m_image = &CreateWidget<OvUI::Widgets::Visual::Image>(tex->GetID(), OvMaths::FVector2{0.f, 0.f});
	scrollable = false;
}

void OvEditor::Panels::AView::Update(float p_deltaTime)
{
}

void OvEditor::Panels::AView::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	OvUI::Panels::PanelWindow::_Draw_Impl();
	ImGui::PopStyleVar();
}

void OvEditor::Panels::AView::InitFrame()
{
	m_renderer->AddDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>(
		CreateSceneDescriptor()
	);
}

void OvEditor::Panels::AView::Render()
{
	auto [winWidth, winHeight] = GetSafeSize();
	m_image->size = OvMaths::FVector2(static_cast<float>(winWidth), static_cast<float>(winHeight));

	auto camera = GetCamera();
	auto scene = GetScene();

	if (winWidth > 0 && winHeight > 0 && camera && scene)
	{
		m_framebuffer.Resize(winWidth, winHeight);

		InitFrame();

		OvRendering::Data::FrameDescriptor frameDescriptor;
		frameDescriptor.renderWidth = winWidth;
		frameDescriptor.renderHeight = winHeight;
		frameDescriptor.camera = camera;
		frameDescriptor.outputBuffer = m_framebuffer;

		m_renderer->BeginFrame(frameDescriptor);
		DrawFrame();
		m_renderer->EndFrame();
	}
}

void OvEditor::Panels::AView::DrawFrame()
{
	m_renderer->DrawFrame();
}

std::pair<uint16_t, uint16_t> OvEditor::Panels::AView::GetSafeSize() const
{
	constexpr float kTitleBarHeight = 25.0f; // <--- this takes into account the imgui window title bar
	const auto& size = GetSize();
	return {
		static_cast<uint16_t>(size.x),
		static_cast<uint16_t>(std::max(0.0f, size.y - kTitleBarHeight)) // <--- clamp to prevent the output size to be negative
	}; 
}

const OvCore::Rendering::SceneRenderer& OvEditor::Panels::AView::GetRenderer() const
{
	return *m_renderer.get();
}

OvCore::Rendering::SceneRenderer::SceneDescriptor OvEditor::Panels::AView::CreateSceneDescriptor()
{
	auto scene = GetScene();

	OVASSERT(scene, "No scene assigned to this view!");

	return {
		*scene
	};
}
