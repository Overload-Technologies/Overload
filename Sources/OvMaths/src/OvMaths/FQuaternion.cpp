/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "OvMaths/FQuaternion.h"

#undef PI

#define PI (4.f * atan(1.f))
#define TO_RADIANS(value) value * PI / 180.f
#define TO_DEGREES(value) value * 180.f / PI

const OvMaths::FQuaternion OvMaths::FQuaternion::Identity = OvMaths::FQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

OvMaths::FQuaternion::FQuaternion() :
	x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{

}

OvMaths::FQuaternion::FQuaternion(float p_real) :
	x(0.0f), y(0.0f), z(0.0f), w(p_real)
{
}

OvMaths::FQuaternion::FQuaternion(float p_x, float p_y, float p_z, float p_w) :
	x(p_x), y(p_y), z(p_z), w(p_w)
{
}

OvMaths::FQuaternion::FQuaternion(const FQuaternion & p_other) :
	x(p_other.x), y(p_other.y), z(p_other.z), w(p_other.w)
{
}

OvMaths::FQuaternion::FQuaternion(const FMatrix3& p_rotationMatrix)
{
	float trace = p_rotationMatrix.data[0] + p_rotationMatrix.data[4] + p_rotationMatrix.data[8];
	if (trace > 0.0f)
	{
		// I changed M_EPSILON to 0
		float s = 0.5f / sqrt(trace + 1.0f);
		w = 0.25f / s;
		x = (p_rotationMatrix.data[7] - p_rotationMatrix.data[5]) * s;
		y = (p_rotationMatrix.data[2] - p_rotationMatrix.data[6]) * s;
		z = (p_rotationMatrix.data[3] - p_rotationMatrix.data[1]) * s;
	}
	else
	{
		if (p_rotationMatrix.data[0] > p_rotationMatrix.data[4] && p_rotationMatrix.data[0] > p_rotationMatrix.data[8])
		{
			float s = 2.0f * sqrt(1.0f + p_rotationMatrix.data[0] - p_rotationMatrix.data[4] - p_rotationMatrix.data[8]);
			w = (p_rotationMatrix.data[7] - p_rotationMatrix.data[5]) / s;
			x = 0.25f * s;
			y = (p_rotationMatrix.data[1] + p_rotationMatrix.data[3]) / s;
			z = (p_rotationMatrix.data[2] + p_rotationMatrix.data[6]) / s;
		}
		else if (p_rotationMatrix.data[4] > p_rotationMatrix.data[8])
		{
			float s = 2.0f * sqrt(1.0f + p_rotationMatrix.data[4] - p_rotationMatrix.data[0] - p_rotationMatrix.data[8]);
			w = (p_rotationMatrix.data[2] - p_rotationMatrix.data[6]) / s;
			x = (p_rotationMatrix.data[1] + p_rotationMatrix.data[3]) / s;
			y = 0.25f * s;
			z = (p_rotationMatrix.data[5] + p_rotationMatrix.data[7]) / s;
		}
		else
		{
			float s = 2.0f * sqrt(1.0f + p_rotationMatrix.data[8] - p_rotationMatrix.data[0] - p_rotationMatrix.data[4]);
			w = (p_rotationMatrix.data[3] - p_rotationMatrix.data[1]) / s;
			x = (p_rotationMatrix.data[2] + p_rotationMatrix.data[6]) / s;
			y = (p_rotationMatrix.data[5] + p_rotationMatrix.data[7]) / s;
			z = 0.25f * s;
		}
	}
}

OvMaths::FQuaternion::FQuaternion(const FMatrix4 & p_rotationMatrix)
{
	float halfSquare;

	// Check diagonal (trace)
	const float trace = p_rotationMatrix.data[0] + p_rotationMatrix.data[5] + p_rotationMatrix.data[10];

	if (trace > 0.0f)
	{
		const float InvSquare = 1 / sqrt(trace + 1.f);
		w = 0.5f * (1.f / InvSquare);
		halfSquare = 0.5f * InvSquare;

		x = (p_rotationMatrix.data[6] - p_rotationMatrix.data[9]) * halfSquare;
		y = (p_rotationMatrix.data[8] - p_rotationMatrix.data[2]) * halfSquare;
		z = (p_rotationMatrix.data[1] - p_rotationMatrix.data[4]) * halfSquare;
	}
	else
	{
		// diagonal is negative
		int8_t i = 0;

		if (p_rotationMatrix.data[5] > p_rotationMatrix.data[0])
			i = 1;

		if (p_rotationMatrix.data[10] > p_rotationMatrix.data[0] || p_rotationMatrix.data[10] > p_rotationMatrix.data[5])
			i = 2;

		static const int8_t next[3] = { 1, 2, 0 };
		const int8_t j = next[i];
		const int8_t k = next[j];

		halfSquare = p_rotationMatrix.data[i * 5] - p_rotationMatrix.data[j * 5] - p_rotationMatrix.data[k * 5] + 1.0f;

		const float InvSquare = 1 / sqrt(trace + 1.f);

		float qt[4];
		qt[i] = 0.5f * (1.f / InvSquare);

		halfSquare = 0.5f * InvSquare;
		//if i is 0, j is 1 and k is 2
		if (i == 0)
		{
			qt[3] = (p_rotationMatrix.data[6] - p_rotationMatrix.data[9]) * halfSquare;
			qt[j] = (p_rotationMatrix.data[1] + p_rotationMatrix.data[4]) * halfSquare;
			qt[k] = (p_rotationMatrix.data[2] + p_rotationMatrix.data[8]) * halfSquare;
		}
		//if i is 1, j is 2 and k is 0
		else if (i == 1)
		{
			qt[3] = (p_rotationMatrix.data[8] - p_rotationMatrix.data[2]) * halfSquare;
			qt[j] = (p_rotationMatrix.data[6] + p_rotationMatrix.data[9]) * halfSquare;
			qt[k] = (p_rotationMatrix.data[4] + p_rotationMatrix.data[1]) * halfSquare;
		}
		//if i is 2, j is 0 and k is 1
		else
		{
			qt[3] = (p_rotationMatrix.data[1] - p_rotationMatrix.data[4]) * halfSquare;
			qt[j] = (p_rotationMatrix.data[8] + p_rotationMatrix.data[2]) * halfSquare;
			qt[k] = (p_rotationMatrix.data[9] + p_rotationMatrix.data[6]) * halfSquare;
		}
		x = qt[0];
		y = qt[1];
		z = qt[2];
		w = qt[3];
	}
}

OvMaths::FQuaternion::FQuaternion(const FVector3 & p_euler)
{
	/* Degree to radians then times 0.5f = 0.0087f */
	float yaw	= TO_RADIANS(p_euler.z) * 0.5f;
	float pitch = TO_RADIANS(p_euler.y) * 0.5f;
	float roll	= TO_RADIANS(p_euler.x) * 0.5f;

	float cy = cos(yaw);
	float sy = sin(yaw);
	float cp = cos(pitch);
	float sp = sin(pitch);
	float cr = cos(roll);
	float sr = sin(roll);

	x = sr * cp * cy - cr * sp * sy;
	y = cr * sp * cy + sr * cp * sy;
	z = cr * cp * sy - sr * sp * cy;
	w = cr * cp * cy + sr * sp * sy;
}

OvMaths::FQuaternion OvMaths::FQuaternion::LookAt(const FVector3& p_forward, const FVector3& p_up)
{
	auto vector = OvMaths::FVector3::Normalize(p_forward);
	auto vector2 = OvMaths::FVector3::Normalize(OvMaths::FVector3::Cross(p_up, vector));
	auto vector3 = OvMaths::FVector3::Cross(vector, vector2);
	auto m00 = vector2.x;
	auto m01 = vector2.y;
	auto m02 = vector2.z;
	auto m10 = vector3.x;
	auto m11 = vector3.y;
	auto m12 = vector3.z;
	auto m20 = vector.x;
	auto m21 = vector.y;
	auto m22 = vector.z;


	float num8 = (m00 + m11) + m22;
	auto quaternion = OvMaths::FQuaternion::Identity;
	if (num8 > 0.f)
	{
		auto num = sqrt(num8 + 1.f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		auto num7 = sqrt(((1.f + m00) - m11) - m22);
		auto num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		auto num6 = sqrt(((1.f + m11) - m00) - m22);
		auto num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	auto num5 = sqrt(((1.f + m22) - m00) - m11);
	auto num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

bool OvMaths::FQuaternion::IsIdentity(const FQuaternion & p_target)
{
	return p_target.w == 1.0f && Length(p_target) == 1.0f;
}

bool OvMaths::FQuaternion::IsPure(const FQuaternion & p_target)
{
	return p_target.w == 0.0f && (p_target.x != 0.0f || p_target.y != 0.0f || p_target.z != 0.0f);
}

bool OvMaths::FQuaternion::IsNormalized(const FQuaternion & p_target)
{
	return abs(Length(p_target) - 1.0f) < 0.0001f;
}

float OvMaths::FQuaternion::DotProduct(const FQuaternion & p_left, const FQuaternion & p_right)
{
	return 
		p_left.x * p_right.x +
		p_left.y * p_right.y +
		p_left.z * p_right.z +
		p_left.w * p_right.w;
}

OvMaths::FQuaternion OvMaths::FQuaternion::Normalize(const FQuaternion & p_target)
{
	return p_target / Length(p_target);
}

float OvMaths::FQuaternion::Length(const FQuaternion & p_target)
{
	return sqrtf(LengthSquare(p_target));
}

float OvMaths::FQuaternion::LengthSquare(const FQuaternion & p_target)
{
	return p_target.x * p_target.x + p_target.y * p_target.y + p_target.z * p_target.z + p_target.w * p_target.w;
}

float OvMaths::FQuaternion::GetAngle(const FQuaternion & p_target)
{
	return 2.0f * acos(p_target.w);
}

OvMaths::FVector3 OvMaths::FQuaternion::GetRotationAxis(const FQuaternion & p_target)
{
	const float S = sqrt(std::max(1.f - (p_target.w * p_target.w), 0.f));

	if (S >= 0.0001f)
	{
		return FVector3(p_target.x / S, p_target.y / S, p_target.z / S);
	}

	return FVector3(1.f, 0.f, 0.f);
}

OvMaths::FQuaternion OvMaths::FQuaternion::Inverse(const FQuaternion & p_target)
{
	return Conjugate(p_target) / LengthSquare(p_target);
}

OvMaths::FQuaternion OvMaths::FQuaternion::Conjugate(const FQuaternion & p_target)
{
	return { -p_target.x, -p_target.y, -p_target.z, p_target.w };
}

OvMaths::FQuaternion OvMaths::FQuaternion::Square(const FQuaternion & p_target)
{
	return p_target * p_target;
}

std::pair<OvMaths::FVector3, float> OvMaths::FQuaternion::GetAxisAndAngle(const FQuaternion & p_target)
{
	return std::pair<FVector3, float>(GetRotationAxis(p_target), GetAngle(p_target));
}

float OvMaths::FQuaternion::AngularDistance(const FQuaternion& p_left, const FQuaternion & p_right)
{
	float innerProd = (p_left | p_right);
	return acos((2.0f * innerProd * innerProd) - 1.0f);
}

OvMaths::FQuaternion OvMaths::FQuaternion::Lerp(const FQuaternion& p_start, const FQuaternion& p_end, float p_alpha)
{
	p_alpha = std::clamp(p_alpha, 0.f, 1.f);

	FQuaternion q;

	if (FQuaternion::DotProduct(p_start, p_end) < 0.f)
	{
		q.x = p_start.x + p_alpha * (-p_end.x - p_start.x);
		q.y = p_start.y + p_alpha * (-p_end.y - p_start.y);
		q.z = p_start.z + p_alpha * (-p_end.z - p_start.z);
		q.w = p_start.w + p_alpha * (-p_end.w - p_start.w);
	}
	else
	{
		q.x = p_start.x + (p_end.x - p_start.x) * p_alpha;
		q.y = p_start.y + (p_end.y - p_start.y) * p_alpha;
		q.z = p_start.z + (p_end.z - p_start.z) * p_alpha;
		q.w = p_start.w + (p_end.w - p_start.w) * p_alpha;
	}

	return FQuaternion::Normalize(q);
}

OvMaths::FQuaternion OvMaths::FQuaternion::Slerp(const FQuaternion& p_start, const FQuaternion& p_end, float p_alpha)
{
	FQuaternion from = p_start;
	FQuaternion to = p_end;

	p_alpha = std::clamp(p_alpha, 0.f, 1.f);
	float cosAngle = FQuaternion::DotProduct(from, to);

	if (cosAngle < 0.f)
	{
		cosAngle = -cosAngle;
		to = FQuaternion(-to.x, -to.y, -to.z, -to.w);
	}

	if (cosAngle < 0.95f)
	{
		float angle = std::acos(cosAngle);
		float sinAngle = std::sin(angle);
		float invSinAngle = 1.f / sinAngle;
		float t1 = std::sin((1 - p_alpha) * angle) * invSinAngle;
		float t2 = std::sin(p_alpha * angle) * invSinAngle;
		return FQuaternion(from.x * t1 + to.x * t2, from.y * t1 + to.y * t2, from.z * t1 + to.z * t2, from.w * t1 + to.w * t2);
	}
	else
	{
		return FQuaternion::Lerp(from, to, p_alpha);
	}
}

OvMaths::FQuaternion OvMaths::FQuaternion::Nlerp(const FQuaternion & p_start, const FQuaternion & p_end, float p_alpha)
{
	return Normalize(Lerp(p_start, p_end, p_alpha));
}

OvMaths::FVector3 OvMaths::FQuaternion::RotatePoint(const FVector3& p_point, const FQuaternion& p_quaternion)
{
	FVector3 Q(p_quaternion.x, p_quaternion.y, p_quaternion.z);
	FVector3 T = FVector3::Cross(Q, p_point) * 2.0f;

	return p_point + (T * p_quaternion.w) + FVector3::Cross(Q, T);
}

OvMaths::FVector3 OvMaths::FQuaternion::RotatePoint(const FVector3 & p_point, const FQuaternion & p_quaternion, const FVector3 & p_pivot)
{
	FVector3 toRotate = p_point - p_pivot;
	return RotatePoint(toRotate, p_quaternion);
}

OvMaths::FVector3 OvMaths::FQuaternion::EulerAngles(const FQuaternion& p_target)
{
	// This is a kind of hack because when the input Quaternion is {0.5f, 0.5f, -0.5f, 0.5f} or
	// {0.5f, 0.5f, 0.5f, -0.5f}, the output value is incorrect.
	if (p_target == FQuaternion{0.5f, 0.5f, -0.5f, 0.5f}) return { 90.0f, 90.0f, 0.0f };
	if (p_target == FQuaternion{0.5f, 0.5f, 0.5f, -0.5f}) return { -90.0f, -90.0f, 0.0f };

	// roll (x-axis rotation)
	const float sinr_cosp = +2.0f * (p_target.w * p_target.x + p_target.y * p_target.z);
	const float cosr_cosp = +1.0f - 2.0f * (p_target.x * p_target.x + p_target.y * p_target.y);
	const float roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float pitch = 0.f;
	const float sinp = +2.0f * (p_target.w * p_target.y - p_target.z * p_target.x);
	if (fabs(sinp) >= 1)
		pitch = static_cast<float>(copysign(PI / 2.0f, sinp)); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	const float siny_cosp = +2.0f * (p_target.w * p_target.z + p_target.x * p_target.y);
	const float cosy_cosp = +1.0f - 2.0f * (p_target.y * p_target.y + p_target.z * p_target.z);
	const float yaw = atan2(siny_cosp, cosy_cosp);

	return TO_DEGREES(FVector3(roll, pitch, yaw)); // XYZ
}

OvMaths::FMatrix3 OvMaths::FQuaternion::ToMatrix3(const FQuaternion & p_target)
{
	if (!IsNormalized(p_target))
		throw std::logic_error("Cannot convert non-normalized quaternions to Matrix4");

	float y2 = p_target.y * p_target.y;
	float wz = p_target.w * p_target.z;
	float x2 = p_target.x * p_target.x;
	float z2 = p_target.z * p_target.z;
	float xz = p_target.x * p_target.z;
	float yz = p_target.y * p_target.z;
	float xy = p_target.x * p_target.y;
	float wy = p_target.w * p_target.y;
	float wx = p_target.w * p_target.x;

	FMatrix3 converted;
	converted.data[0] = 1.0f - (2 * y2) - (2 * z2);
	converted.data[1] = (2 * xy) - (2 * wz);
	converted.data[2] = (2 * xz) + (2 * wy);
	converted.data[3] = (2 * xy) + (2 * wz);
	converted.data[4] = 1.0f - (2 * x2) - (2 * z2);
	converted.data[5] = (2 * yz) - (2 * wx);
	converted.data[6] = (2 * xz) - (2 * wy);
	converted.data[7] = (2 * yz) + (2 * wx);
	converted.data[8] = 1.0f - (2 * x2) - (2 * y2);
	return converted;
}

OvMaths::FMatrix4 OvMaths::FQuaternion::ToMatrix4(const FQuaternion & p_target)
{
	if (!IsNormalized(p_target))
		throw std::logic_error("Cannot convert non-normalized quaternions to Matrix4");

	float y2 = p_target.y * p_target.y;	float wz = p_target.w * p_target.z;	float x2 = p_target.x * p_target.x;
	float z2 = p_target.z * p_target.z;	float xz = p_target.x * p_target.z;	float yz = p_target.y * p_target.z;
	float xy = p_target.x * p_target.y;	float wy = p_target.w * p_target.y;	float wx = p_target.w * p_target.x;

	FMatrix4 converted;
	converted.data[0] = 1.0f - (2 * y2) - (2 * z2);		converted.data[1] = (2 * xy) - (2 * wz);				converted.data[2] = (2 * xz) + (2 * wy);			 converted.data[3] = 0;
	converted.data[4] = (2 * xy) + (2 * wz);				converted.data[5] = 1.0f - (2 * x2) - (2 * z2);		converted.data[6] = (2 * yz) - (2 * wx);			 converted.data[7] = 0;
	converted.data[8] = (2 * xz) - (2 * wy);				converted.data[9] = (2 * yz) + (2 * wx);			converted.data[10] = 1.0f - (2 * x2) - (2 * y2); converted.data[11] = 0;
	converted.data[12] = 0;								converted.data[13] = 0;								converted.data[14] = 0;							 converted.data[15] = 1;
	return converted;
}

bool OvMaths::FQuaternion::operator==(const FQuaternion& p_otherQuat) const
{
	return x == p_otherQuat.x && y == p_otherQuat.x && z == p_otherQuat.z && w == p_otherQuat.w;
}

bool OvMaths::FQuaternion::operator!=(const FQuaternion& p_otherQuat) const
{
	return x != p_otherQuat.x || y != p_otherQuat.x || z != p_otherQuat.z || w != p_otherQuat.w;
}

OvMaths::FQuaternion OvMaths::FQuaternion::operator+(const FQuaternion& p_otherQuat) const
{
	return FQuaternion(x + p_otherQuat.x, y + p_otherQuat.x,
		z + p_otherQuat.z, w + p_otherQuat.w);
}

OvMaths::FQuaternion& OvMaths::FQuaternion::operator+=(const FQuaternion& p_otherQuat)
{
	x += p_otherQuat.x;
	y += p_otherQuat.x;
	z += p_otherQuat.z;
	w += p_otherQuat.w;
	return *this;
}

OvMaths::FQuaternion OvMaths::FQuaternion::operator-(const FQuaternion& p_otherQuat) const
{
	return FQuaternion(x - p_otherQuat.x, y - p_otherQuat.x,
		z - p_otherQuat.z, w - p_otherQuat.w);
}

OvMaths::FQuaternion& OvMaths::FQuaternion::operator-=(const FQuaternion& p_otherQuat)
{
	x -= p_otherQuat.x;
	y -= p_otherQuat.x;
	z -= p_otherQuat.z;
	w -= p_otherQuat.w;
	return *this;
}

float OvMaths::FQuaternion::operator|(const FQuaternion& p_otherQuat) const
{
	return (x * p_otherQuat.x + y * p_otherQuat.x + z *
		p_otherQuat.z + w * p_otherQuat.w);
}

OvMaths::FQuaternion OvMaths::FQuaternion::operator*(const float p_scale) const
{
	FQuaternion result(*this);
	result.x *= p_scale;
	result.y *= p_scale;
	result.z *= p_scale;
	result.w *= p_scale;

	return result;
}

OvMaths::FQuaternion& OvMaths::FQuaternion::operator*=(const float p_scale)
{
	x *= p_scale;
	y *= p_scale;
	z *= p_scale;
	w *= p_scale;

	return *this;
}

OvMaths::FQuaternion OvMaths::FQuaternion::operator*(const FQuaternion& p_otherQuat) const
{
	return FQuaternion
	(
		x * p_otherQuat.w + y * p_otherQuat.z - z * p_otherQuat.y + w * p_otherQuat.x,
		-x * p_otherQuat.z + y * p_otherQuat.w + z * p_otherQuat.x + w * p_otherQuat.y,
		x * p_otherQuat.y - y * p_otherQuat.x + z * p_otherQuat.w + w * p_otherQuat.z,
		-x * p_otherQuat.x - y * p_otherQuat.y - z * p_otherQuat.z + w * p_otherQuat.w
	);
}

OvMaths::FQuaternion& OvMaths::FQuaternion::operator*=(const FQuaternion& p_otherQuat)
{
	FQuaternion temp(
		x * p_otherQuat.w + y * p_otherQuat.z - z * p_otherQuat.x +
		w * p_otherQuat.x,
		-x * p_otherQuat.z + y * p_otherQuat.w + z * p_otherQuat.x +
		w * p_otherQuat.x,
		x * p_otherQuat.x - y * p_otherQuat.x + z * p_otherQuat.w -
		w * p_otherQuat.z,
		-x * p_otherQuat.x - y * p_otherQuat.x - z * p_otherQuat.z +
		w * p_otherQuat.w);

	x = temp.x;
	y = temp.y;
	z = temp.z;
	w = temp.w;

	return *this;
}

OvMaths::FVector3 OvMaths::FQuaternion::operator*(const FVector3& p_toMultiply) const
{
	const float num = x * 2.0f;
	const float num2 = y * 2.0f;
	const float num3 = z * 2.0f;
	const float num4 = x * num;
	const float num5 = y * num2;
	const float num6 = z * num3;
	const float num7 = x * num2;
	const float num8 = x * num3;
	const float num9 = y * num3;
	const float num10 = w * num;
	const float num11 = w * num2;
	const float num12 = w * num3;
	FVector3 result;
	result.x = (1.f - (num5 + num6)) * p_toMultiply.x + (num7 - num12) * p_toMultiply.y + (num8 + num11) *
		p_toMultiply.z;
	result.y = (num7 + num12) * p_toMultiply.x + (1.f - (num4 + num6)) * p_toMultiply.y + (num9 - num10) *
		p_toMultiply.z;
	result.z = (num8 - num11) * p_toMultiply.x + (num9 + num10) * p_toMultiply.y + (1.f - (num4 + num5)) *
		p_toMultiply.z;
	return result;
}

OvMaths::FMatrix3 OvMaths::FQuaternion::operator*(const FMatrix3& p_multiply) const
{
	return (ToMatrix3(*this) * p_multiply);
}

OvMaths::FQuaternion& OvMaths::FQuaternion::operator/=(const float p_scale)
{
	const float reciprocate = 1.0f / p_scale;
	x *= reciprocate;
	y *= reciprocate;
	z *= reciprocate;
	w *= reciprocate;

	return *this;
}

OvMaths::FQuaternion OvMaths::FQuaternion::operator/(const float p_scale) const
{
	FQuaternion temp(*this);
	const float reciprocate = 1.0f / p_scale;
	temp.x *= reciprocate;
	temp.y *= reciprocate;
	temp.z *= reciprocate;
	temp.w *= reciprocate;

	return temp;
}