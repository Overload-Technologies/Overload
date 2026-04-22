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

OvUI::Types::ColorEffector OvUI::Types::ColorEffector::Ref(const Color& p_styleColor)
{
	ColorEffector effector;
	effector.m_source = &p_styleColor;
	return effector;
}

OvUI::Types::ColorEffector OvUI::Types::ColorEffector::Ref(const Color& p_styleColor, const Color& p_tint)
{
	ColorEffector effector;
	effector.m_source = &p_styleColor;
	effector.tint = p_tint;
	return effector;
}

bool OvUI::Types::ColorEffector::HasSource() const
{
	return !std::holds_alternative<std::monostate>(m_source);
}

OvUI::Types::Color OvUI::Types::ColorEffector::Resolve() const
{
	const Color* source = nullptr;

	if (std::holds_alternative<Color>(m_source))
		source = &std::get<Color>(m_source);
	else if (std::holds_alternative<const Color*>(m_source))
		source = std::get<const Color*>(m_source);
	else
		return tint; // fallback: no source set

	return Color{
		source->r * tint.r,
		source->g * tint.g,
		source->b * tint.b,
		source->a * tint.a
	};
}
