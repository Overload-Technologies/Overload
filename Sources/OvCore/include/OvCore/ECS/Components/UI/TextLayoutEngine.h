/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string_view>
#include <vector>

#include <OvMaths/FVector2.h>
#include <OvRendering/Resources/Font.h>

namespace OvCore::ECS::Components::UI
{
	/**
	* Resolves text wrapping, alignment and bounds without creating render geometry.
	*/
	class TextLayoutEngine
	{
	public:
		enum class EHorizontalAlignment
		{
			LEFT,
			CENTER,
			RIGHT
		};

		enum class EVerticalAlignment
		{
			TOP,
			CENTER,
			BOTTOM
		};

		struct Input
		{
			std::string_view text;
			OvRendering::Resources::Font* font = nullptr;
			float fontSize = 1.0f;
			OvMaths::FVector2 uiSize = OvMaths::FVector2::Zero;
			EHorizontalAlignment horizontalAlignment = EHorizontalAlignment::LEFT;
			EVerticalAlignment verticalAlignment = EVerticalAlignment::TOP;
		};

		struct Glyph
		{
			float left = 0.0f;
			float right = 0.0f;
			float bottom = 0.0f;
			float top = 0.0f;
			float uMin = 0.0f;
			float uMax = 0.0f;
			float vMin = 0.0f;
			float vMax = 0.0f;
		};

		struct Output
		{
			OvMaths::FVector2 size = OvMaths::FVector2::Zero;
			OvMaths::FVector2 contentSize = OvMaths::FVector2::Zero;
			std::vector<Glyph> glyphs;
		};

		static Output Layout(const Input& p_input);
	};
}
