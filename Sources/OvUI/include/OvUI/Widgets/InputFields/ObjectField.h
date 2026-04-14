/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <OvTools/Eventing/Event.h>
#include <OvUI/Widgets/DataWidget.h>

namespace OvUI::Widgets::InputFields
{
	/**
	* Generic read-only "object picker" field.
	* Shows a disabled InputText with an optional icon button to open a picker,
	* and an optional framed preview image below (for texture-like assets).
	*
	* @tparam T  Tracked value type. Common uses:
	*            - std::string  for asset paths (AssetField)
	*            - uint64_t     for actor GUIDs  (ActorField)
	*/
	template<typename T>
	class ObjectField : public DataWidget<T>
	{
	public:
		ObjectField(T p_content = {}, std::string p_displayName = "");

	protected:
		void _Draw_Impl() override;

	public:
		T content;

		/** Human-readable label shown in the text box. Used when displayFormatter is null. */
		std::string displayName;

		/** Hover tooltip text. Used when tooltipFormatter is null. */
		std::string tooltip;

		/**
		* If set, called every frame to derive the display name from content.
		* Takes priority over displayName.
		*/
		std::function<std::string(const T&)> displayFormatter;

		/**
		* If set, called every frame to derive the tooltip from content.
		* Takes priority over tooltip.
		*/
		std::function<std::string(const T&)> tooltipFormatter;

		uint32_t iconTextureID = 0;

		/** When non-zero, a framed preview image is drawn below the input row. */
		uint32_t previewTextureID = 0;
		float previewSize = 64.0f;

		OvTools::Eventing::Event<> ClickedEvent;
		OvTools::Eventing::Event<> DoubleClickedEvent;
	};

	extern template class ObjectField<std::string>;
	extern template class ObjectField<uint64_t>;
}
