/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Logger.h>

#include <OvRendering/Features/FrameInfoRenderFeature.h>

#include "OvEditor/Panels/FrameInfo.h"
#include "OvEditor/Core/EditorActions.h"

using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvUI::Types;

namespace
{
	constexpr OvRendering::Data::FrameInfo kEmptyFrameInfo{};
	constexpr std::string_view kFramerateText = "FPS: {}";
	constexpr float kFramerateUpdateInterval = (1.0f / 60.0f) * 10.0f;
}

OvEditor::Panels::FrameInfo::FrameInfo
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings),
	m_fpsText(CreateWidget<Texts::Text>(std::format(kFramerateText, 0))),
	m_viewNameText(CreateWidget<Texts::Text>()),

	m_separator(CreateWidget<Visual::Separator>()),

	m_batchCountText(CreateWidget<Texts::Text>("")),
	m_instanceCountText(CreateWidget<Texts::Text>("")),
	m_polyCountText(CreateWidget<Texts::Text>("")),
	m_vertexCountText(CreateWidget<Texts::Text>(""))
{
	m_polyCountText.lineBreak = false;
}

const OvRendering::Data::FrameInfo& GetFrameInfoFromView(const OvEditor::Panels::AView& p_view)
{
	return p_view
		.GetRenderer()
		.GetFeature<OvRendering::Features::FrameInfoRenderFeature>()
		.GetFrameInfo();
}

void OvEditor::Panels::FrameInfo::Update(OvTools::Utils::OptRef<AView> p_targetView, float p_deltaTime)
{
	using namespace OvTools::Utils;

	m_framerateStats.elapsedFrames++;
	m_framerateStats.elapsedTime += p_deltaTime;

	if (m_framerateStats.elapsedTime >= kFramerateUpdateInterval)
	{
		float averageFramerate = m_framerateStats.elapsedFrames / m_framerateStats.elapsedTime;
		m_fpsText.content = std::format(kFramerateText, static_cast<uint32_t>(std::roundf(averageFramerate)));
		m_framerateStats = {};
	}

	m_viewNameText.content = "Target View: " + (p_targetView ? p_targetView.value().name : "None");

	auto& frameInfo = p_targetView ? GetFrameInfoFromView(p_targetView.value()) : kEmptyFrameInfo;

	// Workaround for Tracy memory profiler.
	// When using a non-default locale, std::format
	// will call delete on a non-allocated object,
	// resulting in a instrumentation error, interrupting
	// Tracy profiler's execution.
#if defined(TRACY_MEMORY_ENABLE)
	const std::locale loc{ };
#else
	const std::locale loc{ "" };
#endif

	m_batchCountText.content = std::format(loc, "Batches: {:L}", frameInfo.batchCount);
	m_instanceCountText.content = std::format(loc, "Instances: {:L}", frameInfo.instanceCount);
	m_polyCountText.content = std::format(loc, "Polygons: {:L}", frameInfo.polyCount);
	m_vertexCountText.content = std::format(loc, "Vertices: {:L}", frameInfo.vertexCount);
}
