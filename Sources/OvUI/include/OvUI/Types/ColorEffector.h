/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <variant>

#include <OvUI/Types/Color.h>

namespace OvUI::Types
{
	/**
	* A color source that can hold either a direct color value or a live const-reference to
	* a style color, with an optional multiplicative tint modifier.
	*
	* Usage:
	*   ColorEffector a = Color{1,0,0,1};                  // direct value
	*   ColorEffector b = {1, 0, 0};                       // convenience (r,g,b[,a])
	*   ColorEffector c = ColorEffector::Ref(styleColor);  // live pointer to style
	*   ColorEffector d = ColorEffector::Ref(styleColor, tintColor); // live + tint
	*/
	class ColorEffector
	{
	public:
		using Source = std::variant<std::monostate, Color, const Color*>;

		ColorEffector() = default;
		ColorEffector(const Color& p_value);
		ColorEffector(float p_r, float p_g, float p_b, float p_a = 1.0f);

		/**
		* Create a ColorEffector bound to a live style color reference.
		* The pointer is stored — changes to the referenced color are reflected on Resolve().
		*/
		static ColorEffector Ref(const Color& p_styleColor);
		static ColorEffector Ref(const Color& p_styleColor, const Color& p_tint);

		/**
		* Returns true if a color source is set (not monostate).
		*/
		bool HasSource() const;

		/**
		* Resolves to the final color: source_color * tint.
		* Only call this when HasSource() is true (or when a default value is guaranteed by construction).
		*/
		Color Resolve() const;

		/**
		* Multiplicative tint applied on top of the source color.
		* Component-wise multiplication: result = source * tint.
		*/
		Color tint{ 1.0f, 1.0f, 1.0f, 1.0f };

	private:
		Source m_source;
	};
}
