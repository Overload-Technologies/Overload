/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/math/Vec3.h>

#include <array>

namespace baregl::math
{
	struct Mat3
	{
		std::array<Vec3, 4> data {{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f}
		}};

		Vec3& operator[](int index)
		{
			return data[index];
		}

		const Vec3& operator[](int index) const
		{
			return data[index];
		}
	};
}
