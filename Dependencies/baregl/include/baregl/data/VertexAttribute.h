/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/types/EDataType.h>

#include <initializer_list>
#include <variant>

namespace baregl::data
{
	/**
	* Vertex attribute using glVertexAttribPointer (data converted to float).
	*/
	struct FloatVertexAttribute
	{
		types::EDataType type = types::EDataType::FLOAT;
		uint8_t count = 4;
		bool normalized = false;
	};

	/**
	* Vertex attribute using glVertexAttribIPointer (integer data kept as-is).
	* Valid types: BYTE, UNSIGNED_BYTE, SHORT, UNSIGNED_SHORT, INT, UNSIGNED_INT.
	*/
	struct IntegerVertexAttribute
	{
		types::EDataType type = types::EDataType::INT;
		uint8_t count = 4;
	};

	/**
	* Vertex attribute using glVertexAttribLPointer (double-precision data kept as-is).
	*/
	struct DoubleVertexAttribute
	{
		uint8_t count = 4;
	};

	using VertexAttribute = std::variant<FloatVertexAttribute, IntegerVertexAttribute, DoubleVertexAttribute>;
	using VertexAttributeLayout = std::initializer_list<VertexAttribute>;
}
