/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <imgui.h>

#include <OvTools/Eventing/Event.h>

#include <OvUI/Widgets/DataWidget.h>

namespace OvUI::Widgets::Drags
{
	/**
	* Drag widget of generic type
	*/
	template <typename T>
	class DragSingleScalar : public DataWidget<T>
	{
		static_assert(std::is_scalar<T>::value, "Invalid DragSingleScalar T (Scalar expected)");

	public:
		/**
		* Constructor
		* @param p_dataType
		* @param p_min
		* @param p_max
		* @param p_value
		* @param p_speed
		* @param p_label
		* @param p_format
		*/
		DragSingleScalar
		(
			ImGuiDataType p_dataType,
			T p_min,
			T p_max,
			T p_value,
			float p_speed,
			const std::string& p_label,
			const std::string& p_format
		) : DataWidget<T>(value), m_dataType(p_dataType), min(p_min), max(p_max), value(p_value), speed(p_speed) , label(p_label), format(p_format) {}

	protected:
		void _Draw_Impl() override
		{
			if (max < min)
				max = min;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;

			if (ImGui::DragScalar((label + this->m_widgetID).c_str(), m_dataType, &value, speed, &min, &max, format.c_str()))
			{
				ValueChangedEvent.Invoke(value);
				this->NotifyChange();
			}
		}

	public:
		T min;
		T max;
		T value;
		float speed;
		std::string label;
		std::string format;
		OvTools::Eventing::Event<T> ValueChangedEvent;

	private:
		ImGuiDataType m_dataType;
	};
}