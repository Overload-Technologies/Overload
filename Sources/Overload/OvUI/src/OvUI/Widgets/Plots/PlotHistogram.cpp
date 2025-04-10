/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Plots/PlotHistogram.h"
#include "OvUI/Internal/Converter.h"

OvUI::Widgets::Plots::PlotHistogram::PlotHistogram
(
	const std::vector<float>& p_data,
	float p_minScale,
	float p_maxScale,
	const OvMaths::FVector2& p_size,
	const std::string& p_overlay,
	const std::string& p_label,
	int p_forceHover
) : APlot(p_data, p_minScale, p_maxScale, p_size, p_overlay, p_label, p_forceHover)
{
}

void OvUI::Widgets::Plots::PlotHistogram::_Draw_Impl()
{
	// TODO: Use forceHover
	ImGui::PlotHistogram((label + m_widgetID).c_str(), data.data(), static_cast<int>(data.size()), 0, overlay.c_str(), minScale, maxScale, Internal::Converter::ToImVec2(size), sizeof(float));
}
