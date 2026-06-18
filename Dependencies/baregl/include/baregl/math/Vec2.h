/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

namespace baregl::math
{
	struct Vec2
	{
		float x = 0.0f;
		float y = 0.0f;

		float& operator[](int index)
		{
			return (&x)[index];
		}

		const float& operator[](int index) const
		{
			return (&x)[index];
		}
	};
}
