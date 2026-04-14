/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>

#include <OvTools/Eventing/Event.h>
#include <OvUI/Widgets/DataWidget.h>

namespace OvUI::Widgets::InputFields
{
	/**
	* Read-only input field that displays an actor name and includes a picker button.
	* The stored value is the actor's GUID (0 = none).
	*/
	class ActorField : public DataWidget<uint64_t>
	{
	public:
		ActorField(uint64_t p_guid = 0, const std::string& p_displayName = "");

	protected:
		void _Draw_Impl() override;

	public:
		uint64_t guid = 0;
		std::string displayName;
		uint32_t iconTextureID = 0;

		OvTools::Eventing::Event<> ClickedEvent;
		OvTools::Eventing::Event<> DoubleClickedEvent;
	};
}
