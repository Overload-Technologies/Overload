/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector2.h>

#include "OvUI/Internal/TextureID.h"
#include "OvUI/Types/Color.h"
#include "OvUI/Widgets/Buttons/AButton.h"

namespace OvUI::Widgets::Buttons
{
	/**
	* Button widget with an image
	*/
	class ButtonImage : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_textureID
		* @param p_size
		*/
		ButtonImage(uint32_t p_textureID, const OvMaths::FVector2& p_size);

	protected:
		void _Draw_Impl() override;

	public:
		bool disabled = false;

		OvUI::Types::Color background = { 0, 0, 0, 0 };
		OvUI::Types::Color tint = { 1, 1, 1, 1 };

		Internal::TextureID textureID;
		OvMaths::FVector2 size;
	};
}