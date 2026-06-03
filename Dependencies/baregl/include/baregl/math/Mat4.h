/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/math/Vec4.h>

#include <array>

namespace baregl::math
{
	struct Mat4
	{
		std::array<Vec4, 4> data {{
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f}
		}};

		Vec4& operator[](int index)
		{
			return data[index];
		}

		const Vec4& operator[](int index) const
		{
			return data[index];
		}
	};
}
