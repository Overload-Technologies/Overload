/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvCore/Helpers/GUIDrawer.h"

namespace OvCore::Helpers
{
	template<typename T>
	inline ImGuiDataType_ GUIDrawer::GetDataType()
	{
		if constexpr (std::is_same<T, float>::value)			return ImGuiDataType_Float;
		else if constexpr (std::is_same<T, double>::value)		return ImGuiDataType_Double;
		else if constexpr (std::is_same<T, uint8_t>::value)		return ImGuiDataType_U8;
		else if constexpr (std::is_same<T, uint16_t>::value)	return ImGuiDataType_U16;
		else if constexpr (std::is_same<T, uint32_t>::value)	return ImGuiDataType_U32;
		else if constexpr (std::is_same<T, uint64_t>::value)	return ImGuiDataType_U64;
		else if constexpr (std::is_same<T, int8_t>::value)		return ImGuiDataType_S8;
		else if constexpr (std::is_same<T, int16_t>::value)		return ImGuiDataType_S16;
		else if constexpr (std::is_same<T, int32_t>::value)		return ImGuiDataType_S32;
		else if constexpr (std::is_same<T, int64_t>::value)		return ImGuiDataType_S64;
	}

	template<typename T>
	inline std::string GUIDrawer::GetFormat()
	{
		if constexpr (std::is_same<T, double>::value) return "%.5f";
		else if constexpr (std::is_same<T, float>::value) return "%.3f";
		else return "%d";
	}

	template<typename T>
	inline void GUIDrawer::DrawScalar(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, T & p_data, float p_step, T p_min, T p_max)
	{
		static_assert(std::is_scalar<T>::value, "T must be a scalar");

		CreateTitle(p_root, p_name);
		auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragSingleScalar<T>>(GetDataType<T>(), p_min, p_max, p_data, p_step, "", GetFormat<T>());
		auto& dispatcher = widget.template AddPlugin<OvUI::Plugins::DataDispatcher<T>>();
		dispatcher.RegisterReference(p_data);
	}

	template<typename T>
	inline void GUIDrawer::DrawScalar(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<T(void)> p_gatherer, std::function<void(T)> p_provider, float p_step, T p_min, T p_max)
	{
		static_assert(std::is_scalar<T>::value, "T must be a scalar");

		CreateTitle(p_root, p_name);
		auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragSingleScalar<T>>(GetDataType<T>(), p_min, p_max, static_cast<T>(0), p_step, "", GetFormat<T>());
		auto& dispatcher = widget.template AddPlugin<OvUI::Plugins::DataDispatcher<T>>();
		dispatcher.RegisterGatherer(p_gatherer);
		dispatcher.RegisterProvider(p_provider);
	}
}