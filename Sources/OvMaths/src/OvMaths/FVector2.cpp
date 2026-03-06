/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cmath>
#include <stdexcept>
#include <utility>

#include "OvMaths/FVector2.h"

const OvMaths::FVector2 OvMaths::FVector2::One(1.0f, 1.0f);
const OvMaths::FVector2 OvMaths::FVector2::Zero(0.0f, 0.0f);

OvMaths::FVector2::FVector2(float p_x, float p_y) :
	x(p_x), y(p_y)
{
}

OvMaths::FVector2::FVector2(const FVector2& p_toCopy) :
	x(p_toCopy.x), y(p_toCopy.y)
{
}

OvMaths::FVector2 OvMaths::FVector2::operator-() const
{
	return operator*(-1);
}

OvMaths::FVector2 OvMaths::FVector2::operator=(const FVector2& p_other)
{
	this->x = p_other.x;
	this->y = p_other.y;

	return *this;
}

OvMaths::FVector2 OvMaths::FVector2::operator+(const FVector2& p_other) const
{
	return Add(*this, p_other);
}

OvMaths::FVector2& OvMaths::FVector2::operator+=(const FVector2& p_other)
{
	*this = Add(*this, p_other);
	return *this;
}

OvMaths::FVector2 OvMaths::FVector2::operator-(const FVector2& p_other) const
{
	return Substract(*this, p_other);
}

OvMaths::FVector2& OvMaths::FVector2::operator-=(const FVector2& p_other)
{
	*this = Substract(*this, p_other);
	return *this;
}

OvMaths::FVector2 OvMaths::FVector2::operator*(float p_scalar) const
{
	return Multiply(*this, p_scalar);
}

OvMaths::FVector2& OvMaths::FVector2::operator*=(float p_scalar)
{
	*this = Multiply(*this, p_scalar);
	return *this;
}

OvMaths::FVector2 OvMaths::FVector2::operator/(float p_scalar) const
{
	return Divide(*this, p_scalar);
}

OvMaths::FVector2& OvMaths::FVector2::operator/=(float p_scalar)
{
	*this = Divide(*this, p_scalar);
	return *this;
}

bool OvMaths::FVector2::operator==(const FVector2 & p_other)
{
	return
		this->x == p_other.x &&
		this->y == p_other.y;
}

bool OvMaths::FVector2::operator!=(const FVector2 & p_other)
{
	return !operator==(p_other);
}

OvMaths::FVector2 OvMaths::FVector2::Add(const FVector2 & p_left, const FVector2 & p_right)
{
	return FVector2
	(
		p_left.x + p_right.x,
		p_left.y + p_right.y
	);
}

OvMaths::FVector2 OvMaths::FVector2::Substract(const FVector2 & p_left, const FVector2 & p_right)
{
	return FVector2
	(
		p_left.x - p_right.x,
		p_left.y - p_right.y
	);
}

OvMaths::FVector2 OvMaths::FVector2::Multiply(const FVector2& p_target, float p_scalar)
{
	return FVector2
	(
		p_target.x * p_scalar,
		p_target.y * p_scalar
	);
}

OvMaths::FVector2 OvMaths::FVector2::Divide(const FVector2& p_left, float p_scalar)
{
	FVector2 result(p_left);

	if (p_scalar == 0)
		throw std::logic_error("Division by 0");

	result.x /= p_scalar;
	result.y /= p_scalar;

	return result;
}

float OvMaths::FVector2::Length(const FVector2& p_target)
{
	return sqrtf(p_target.x * p_target.x + p_target.y * p_target.y);
}

float OvMaths::FVector2::Dot(const FVector2& p_left, const FVector2& p_right)
{
	return p_left.x * p_right.x + p_left.y * p_right.y;
}

OvMaths::FVector2 OvMaths::FVector2::Normalize(const FVector2& p_target)
{
	float length = Length(p_target);

	if (length > 0.0f)
	{
		float targetLength = 1.0f / length;

		return FVector2
		(
			p_target.x * targetLength,
			p_target.y * targetLength
		);
	}
	else
	{
		return FVector2::Zero;
	}
}

OvMaths::FVector2 OvMaths::FVector2::Lerp(const FVector2 & p_start, const FVector2 & p_end, float p_alpha)
{
	return (p_start + (p_end - p_start) * p_alpha);
}

float OvMaths::FVector2::AngleBetween(const FVector2 & p_from, const FVector2 & p_to)
{
	float lengthProduct = Length(p_from) * Length(p_to);

	if (lengthProduct > 0.0f)
	{
		float fractionResult = Dot(p_from, p_to) / lengthProduct;

		if (fractionResult >= -1.0f && fractionResult <= 1.0f)
			return acosf(fractionResult);
	}

	return 0.0f;
}
