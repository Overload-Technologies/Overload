/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>
#include <variant>

#include <OvUI/Types/Color.h>

namespace OvUI::Types
{
	/**
	* A color source that holds either a direct color value or a live const-reference to
	* a style color, with an optional multiplicative tint modifier.
	*
	* A ColorEffector always has a valid source — it cannot be in a "no color" state.
	*
	* Usage:
	*   ColorEffector a = Color{1,0,0,1};                  // direct value
	*   ColorEffector b = {1, 0, 0};                       // convenience (r,g,b[,a])
	*   ColorEffector c = ColorEffector::Ref(styleColor);  // live ref to style color
	*   ColorEffector d = ColorEffector::Ref(styleColor, tintColor); // live ref + tint
	*/
	class ColorEffector
	{
	public:
		using Source = std::variant<Color, std::reference_wrapper<const Color>>;

		ColorEffector(const Color& p_value);
		ColorEffector(float p_r, float p_g, float p_b, float p_a = 1.0f);

		/**
		* Create a ColorEffector bound to a live style color reference.
		* The reference is stored — changes to the referenced color are reflected on Resolve().
		*/
		static ColorEffector Ref(const Color& p_styleColor);
		static ColorEffector Ref(const Color& p_styleColor, const Color& p_tint);

		/**
		* Resolves to the final color: source_color * tint.
		*/
		Color Resolve() const;

		/**
		* Multiplicative tint applied on top of the source color.
		* Component-wise multiplication: result = source * tint.
		*/
		Color tint{ 1.0f, 1.0f, 1.0f, 1.0f };

	private:
		explicit ColorEffector(std::reference_wrapper<const Color> p_ref);

		Source m_source;
	};
}
