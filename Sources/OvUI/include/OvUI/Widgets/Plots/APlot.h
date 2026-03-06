/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <limits>

#include <OvMaths/FVector2.h>

#include "OvUI/Widgets/AWidget.h"
#include "OvUI/Widgets/DataWidget.h"

namespace OvUI::Widgets::Plots
{
	/**
	* Base class for any plot widget
	*/
	class APlot : public DataWidget<std::vector<float>>
	{
	public:
		/**
		* Constructor
		* @param p_data
		* @param p_minScale
		* @param p_maxScale
		* @param p_size
		* @param p_overlay
		* @param p_label
		* @param p_forceHover
		*/
		APlot
		(
			const std::vector<float>& p_data = std::vector<float>(),
			float p_minScale = std::numeric_limits<float>::min(),
			float p_maxScale = std::numeric_limits<float>::max(),
			const OvMaths::FVector2& p_size = { 0.0f, 0.0f },
			const std::string& p_overlay = "",
			const std::string& p_label = "",
			int p_forceHover = -1
		);

	protected:
		virtual void _Draw_Impl() override = 0;

	public:
		std::vector<float> data;
		float minScale;
		float maxScale;
		OvMaths::FVector2 size;
		std::string overlay;
		std::string label;
		int forceHover;
	};
}