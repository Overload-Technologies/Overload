/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Logger.h>
#include <OvTools/Utils/Format.h>

#include <OvRendering/Features/FrameInfoRenderFeature.h>

#include "OvEditor/Panels/FrameInfo.h"
#include "OvEditor/Core/EditorActions.h"

using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvUI::Types;

constexpr OvRendering::Data::FrameInfo kEmptyFrameInfo{};

OvEditor::Panels::FrameInfo::FrameInfo
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings),

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

void OvEditor::Panels::FrameInfo::Update(OvTools::Utils::OptRef<AView> p_targetView)
{
	using namespace OvTools::Utils;

	m_viewNameText.content = "Target View: " + (p_targetView ? p_targetView.value().name : "None");

	auto& frameInfo = p_targetView ? GetFrameInfoFromView(p_targetView.value()) : kEmptyFrameInfo;

	m_batchCountText.content = std::format("Batches: {}", frameInfo.batchCount);
	m_instanceCountText.content = std::format("Instances: {}", frameInfo.instanceCount);
	m_polyCountText.content = std::format("Polygons: {}", frameInfo.polyCount);
	m_vertexCountText.content = std::format("Vertices: {}", frameInfo.vertexCount);
}
