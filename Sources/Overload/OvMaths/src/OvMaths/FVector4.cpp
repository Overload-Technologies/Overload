/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cmath>
#include <stdexcept>
#include <utility>

#include "OvMaths/FVector4.h"

const OvMaths::FVector4 OvMaths::FVector4::One(1.0f, 1.0f, 1.0f, 1.0f);
const OvMaths::FVector4 OvMaths::FVector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);

OvMaths::FVector4::FVector4(float p_x, float p_y, float p_z, float p_w) : x(p_x), y(p_y), z(p_z), w(p_w)
{
}

OvMaths::FVector4::FVector4(const OvMaths::FVector3& p_vector, float p_w) : x(p_vector.x), y(p_vector.y), z(p_vector.z), w(p_w)
{
}

OvMaths::FVector4::FVector4(const FVector4& p_toCopy) : x(p_toCopy.x), y(p_toCopy.y), z(p_toCopy.z), w(p_toCopy.w)
{
}

OvMaths::FVector4 OvMaths::FVector4::operator-() const
{
	return operator*(-1);
}

OvMaths::FVector4 OvMaths::FVector4::operator=(const FVector4& p_other)
{
	this->x = p_other.x;
	this->y = p_other.y;
	this->z = p_other.z;
	this->w = p_other.w;

	return *this;
}

OvMaths::FVector4 OvMaths::FVector4::operator+(const FVector4& p_other) const
{
	return Add(*this, p_other);
}

OvMaths::FVector4& OvMaths::FVector4::operator+=(const FVector4& p_other)
{
	*this = Add(*this, p_other);
	return *this;
}

OvMaths::FVector4 OvMaths::FVector4::operator-(const FVector4& p_other) const
{
	return Substract(*this, p_other);
}

OvMaths::FVector4& OvMaths::FVector4::operator-=(const FVector4& p_other)
{
	*this = Substract(*this, p_other);
	return *this;
}

OvMaths::FVector4 OvMaths::FVector4::operator*(float p_scalar) const
{
	return Multiply(*this, p_scalar);
}

OvMaths::FVector4& OvMaths::FVector4::operator*=(float p_scalar)
{
	*this = Multiply(*this, p_scalar);
	return *this;
}

OvMaths::FVector4 OvMaths::FVector4::operator/(float p_scalar) const
{
	return Divide(*this, p_scalar);
}

OvMaths::FVector4& OvMaths::FVector4::operator/=(float p_scalar)
{
	*this = Divide(*this, p_scalar);
	return *this;
}

bool OvMaths::FVector4::operator==(const FVector4 & p_other)
{
	return
		this->x == p_other.x &&
		this->y == p_other.y &&
		this->z == p_other.z &&
		this->w == p_other.w;
}

bool OvMaths::FVector4::operator!=(const FVector4 & p_other)
{
	return !operator==(p_other);
}

OvMaths::FVector4 OvMaths::FVector4::Add(const FVector4& p_left, const FVector4& p_right)
{
	return FVector4
	(
		p_left.x + p_right.x,
		p_left.y + p_right.y,
		p_left.z + p_right.z,
		p_left.w + p_right.w
	);
}

OvMaths::FVector4 OvMaths::FVector4::Substract(const FVector4& p_left, const FVector4& p_right)
{
	return FVector4
	(
		p_left.x - p_right.x,
		p_left.y - p_right.y,
		p_left.z - p_right.z,
		p_left.w - p_right.w
	);
}

OvMaths::FVector4 OvMaths::FVector4::Multiply(const FVector4& p_target, float p_scalar)
{
	return FVector4
	(
		p_target.x * p_scalar,
		p_target.y * p_scalar,
		p_target.z * p_scalar,
		p_target.w * p_scalar
	);
}

OvMaths::FVector4 OvMaths::FVector4::Divide(const FVector4& p_left, float p_scalar)
{
	FVector4 result(p_left);

	if (p_scalar == 0)
		throw std::logic_error("Division by 0");

	result.x /= p_scalar;
	result.y /= p_scalar;
	result.z /= p_scalar;
	result.w /= p_scalar;

	return result;
}

float OvMaths::FVector4::Length(const FVector4& p_target)
{
	return sqrtf(p_target.x * p_target.x + p_target.y * p_target.y + p_target.z * p_target.z + p_target.w * p_target.w);
}

float OvMaths::FVector4::Dot(const FVector4& p_left, const FVector4& p_right)
{
	return p_left.x * p_right.x + p_left.y * p_right.y + p_left.z * p_right.z + p_left.w * p_right.w;
}

OvMaths::FVector4 OvMaths::FVector4::Normalize(const FVector4& p_target)
{
	float length = Length(p_target);

	if (length > 0.0f)
	{
		float targetLength = 1.0f / length;

		return FVector4
		(
			p_target.x * targetLength,
			p_target.y * targetLength,
			p_target.z * targetLength,
			p_target.w * targetLength
		);
	}
	else
	{
		return FVector4::Zero;
	}
}

OvMaths::FVector4 OvMaths::FVector4::Lerp(const FVector4& p_start, const FVector4& p_end, float p_alpha)
{
	return (p_start + (p_end - p_start) * p_alpha);
}
