/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector2.h>
#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/DataWidget.h"
#include "OvUI/Widgets/Sliders/ESliderOrientation.h"
#include "OvUI/Internal/Converter.h"

namespace OvUI::Widgets::Sliders
{
	/**
	* Slider widget of generic type
	*/
	template <typename T>
	class SliderSingleScalar : public DataWidget<T>
	{
		static_assert(std::is_scalar<T>::value, "Invalid SliderSingleScalar T (Scalar expected)");

	public:
		/**
		* Constructor
		* @param p_dataType
		* @param p_min
		* @param p_max
		* @param p_value
		* @param p_orientation
		* @param p_label
		* @param p_format
		*/
		SliderSingleScalar
		(
			ImGuiDataType p_dataType,
			T p_min,
			T p_max,
			T p_value,
			ESliderOrientation p_orientation,
			const std::string& p_label,
			const std::string& p_format
		) : DataWidget<T>(value), m_dataType(p_dataType), min(p_min), max(p_max), value(p_value), orientation(p_orientation), label(p_label), format(p_format) {}

	protected:
		void _Draw_Impl() override
		{
			if (max < min)
				max = min;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;

			bool valueChanged = false;

			switch (orientation)
			{
			case ESliderOrientation::HORIZONTAL:
				valueChanged = ImGui::SliderScalar((label + this->m_widgetID).c_str(), m_dataType, &value, &min, &max, format.c_str());
				break;
			case ESliderOrientation::VERTICAL:
				valueChanged = ImGui::VSliderScalar((label + this->m_widgetID).c_str(), Internal::Converter::ToImVec2(verticalModeSize), m_dataType, &value, &min, &max, format.c_str());
				break;
			}

			if (valueChanged)
			{
				ValueChangedEvent.Invoke(value);
				this->NotifyChange();
			}
		}

	public:
		T min;
		T max;
		T value;
		ESliderOrientation orientation;
		OvMaths::FVector2 verticalModeSize; /* Only applied with ESliderOrientation::VERTICAL */
		std::string label;
		std::string format;
		OvTools::Eventing::Event<T> ValueChangedEvent;

	private:
		ImGuiDataType m_dataType;
	};
}