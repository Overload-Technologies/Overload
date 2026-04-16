/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>

#include <OvUI/API/IDrawable.h>
#include <OvUI/Plugins/DataDispatcher.h>
#include <OvUI/Plugins/Pluginable.h>

namespace  OvUI::Internal { class WidgetContainer; }

namespace OvUI::Widgets
{
	/**
	* A AWidget is the base class for any widget of OvUI.
	* It is basically a visual element that can be placed into a panel.
	* It is drawable and can receive plugins
	*/
	class AWidget : public API::IDrawable, public Plugins::Pluginable
	{
	public:
		/**
		* Constructor
		*/
		AWidget();

		/**
		* Draw the widget on the panel
		*/
		virtual void Draw() override;

		/**
		* Link the widget to another one by making its id identical to the given widget
		* @param p_widget
		*/
		void LinkTo(const AWidget& p_widget);

		/**
		* Override the auto-generated widget ID with a custom stable identifier.
		* Useful to preserve ImGui state (e.g. table column widths) across widget
		* rebuilds that would otherwise receive a new monotonic ID.
		* @param p_id  Arbitrary unique string (will be prefixed with "##")
		*/
		void SetID(const std::string& p_id) { m_widgetID = "##" + p_id; }

		/**
		* Mark the widget as destroyed. It will be removed from the owner panel
		* by the garbage collector
		*/
		void Destroy();

		/**
		* Returns true if the widget is marked as destroyed
		*/
		bool IsDestroyed() const;

		/**
		* Defines the parent of this widget
		* @param p_parent
		*/
		void SetParent(Internal::WidgetContainer* p_parent);

		/**
		* Returns true if the widget has a parent
		*/
		bool HasParent() const;

		/**
		* Returns the parent of the widget or nullptr if there is no parent
		*/
		Internal::WidgetContainer* GetParent();

	protected:
		virtual void _Draw_Impl() = 0;
		void BeginDisableOverride();
		void EndDisableOverride();

	public:
		std::string tooltip;
		bool enabled = true;
		bool disabled = false;
		bool lineBreak = true;
		bool neverDisabled = false; // provide a way to bypass the "readonly" setting used by a WidgetContainer

	protected:
		Internal::WidgetContainer* m_parent;
		std::string m_widgetID = "?";
		bool m_autoExecutePlugins = true;
		bool m_skipDisableOverrideLogic = false;

	private:
		static uint64_t __WIDGET_ID_INCREMENT;
		bool m_destroyed = false;
		bool m_isInDisableOverrideScope = false;
	};
}
