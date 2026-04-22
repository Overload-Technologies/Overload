/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Types/ColorEffector.h>

OvUI::Types::ColorEffector::ColorEffector(const Color& p_value)
	: m_source(p_value)
{
}

OvUI::Types::ColorEffector::ColorEffector(float p_r, float p_g, float p_b, float p_a)
	: m_source(Color{ p_r, p_g, p_b, p_a })
{
}

OvUI::Types::ColorEffector::ColorEffector(std::reference_wrapper<const Color> p_ref)
	: m_source(p_ref)
{
}

OvUI::Types::ColorEffector OvUI::Types::ColorEffector::Ref(const Color& p_styleColor)
{
	return ColorEffector(std::ref(p_styleColor));
}

OvUI::Types::ColorEffector OvUI::Types::ColorEffector::Ref(const Color& p_styleColor, const Color& p_tint)
{
	ColorEffector effector(std::ref(p_styleColor));
	effector.tint = p_tint;
	return effector;
}

OvUI::Types::Color OvUI::Types::ColorEffector::Resolve() const
{
	const Color* source = nullptr;

	if (std::holds_alternative<Color>(m_source))
		source = &std::get<Color>(m_source);
	else
		source = &std::get<std::reference_wrapper<const Color>>(m_source).get();

	return Color{
		source->r * tint.r,
		source->g * tint.g,
		source->b * tint.b,
		source->a * tint.a
	};
}
