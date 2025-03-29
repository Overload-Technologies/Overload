/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvTools/Eventing/Event.h>

#include "OvUI/ImGui/imgui.h"

#include "OvUI/Plugins/IPlugin.h"

namespace OvUI::Plugins
{
	/**
	* Represents a drag and drop source
	*/
	template<typename T>
	class DDSource : public IPlugin
	{
	public:
		/**
		* Create the drag and drop source
		* @param p_identifier
		* @param p_tooltip
		* @param p_data
		*/
		DDSource
		(
			const std::string& p_identifier,
			const std::string& p_tooltip,
			T p_data
		) : identifier(p_identifier), tooltip(p_tooltip), data(p_data)
		{}

		/**
		* Execute the behaviour of the drag and drop source
		* @param p_context
		*/
		virtual void Execute(EPluginExecutionContext p_context) override
		{
			ImGuiDragDropFlags src_flags = 0;
			src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
			src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging

			if (!hasTooltip)
				src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip

			if (ImGui::BeginDragDropSource(src_flags))
			{
				if (!m_isDragged)
					DragStartEvent.Invoke();

				m_isDragged = true;

				if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
					ImGui::Text(tooltip.c_str());
				ImGui::SetDragDropPayload(identifier.c_str(), &data, sizeof(data));
				ImGui::EndDragDropSource();
			}
			else
			{
				if (m_isDragged)
					DragStopEvent.Invoke();

				m_isDragged = false;
			}
		}

		/**
		* Returns true if the drag and drop source is dragged
		*/
		bool IsDragged() const
		{
			return m_isDragged;
		}

	public:
		std::string identifier;
		std::string tooltip;
		T data;
		OvTools::Eventing::Event<> DragStartEvent;
		OvTools::Eventing::Event<> DragStopEvent;

		bool hasTooltip = true;

	private:
		bool m_isDragged;
	};
}