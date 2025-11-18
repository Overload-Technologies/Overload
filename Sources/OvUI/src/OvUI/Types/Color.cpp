/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Types/Color.h"

const OvUI::Types::Color OvUI::Types::Color::Red		= { 1.f, 0.f, 0.f };
const OvUI::Types::Color OvUI::Types::Color::Green		= { 0.f, 1.f, 0.f };
const OvUI::Types::Color OvUI::Types::Color::Blue		= { 0.f, 0.f, 1.f };
const OvUI::Types::Color OvUI::Types::Color::White		= { 1.f, 1.f, 1.f };
const OvUI::Types::Color OvUI::Types::Color::Black		= { 0.f, 0.f, 0.f };
const OvUI::Types::Color OvUI::Types::Color::Grey		= { 0.5f, 0.5f, 0.5f };
const OvUI::Types::Color OvUI::Types::Color::Yellow		= { 1.f, 1.f, 0.f };
const OvUI::Types::Color OvUI::Types::Color::Cyan		= { 0.f, 1.f, 1.f };
const OvUI::Types::Color OvUI::Types::Color::Magenta	= { 1.f, 0.f, 1.f };

OvUI::Types::Color::Color(float p_r, float p_g, float p_b, float p_a) : r(p_r), g(p_g), b(p_b), a(p_a)
{
}

OvUI::Types::Color::Color(OvMaths::FVector3 p_vector) : Color(p_vector.x, p_vector.y, p_vector.z)
{
}

OvUI::Types::Color::Color(OvMaths::FVector4 p_vector) : Color(p_vector.x, p_vector.y, p_vector.z, p_vector.w)
{
}

bool OvUI::Types::Color::operator==(const Color & p_other)
{
	return this->r == p_other.r && this->g == p_other.g && this->b == p_other.b && this->a == p_other.a;
}

bool OvUI::Types::Color::operator!=(const Color & p_other)
{
	return !operator==(p_other);
}
