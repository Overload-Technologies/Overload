/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/data/VertexAttribute.h>
#include <baregl/detail/NativeObject.h>
#include <baregl/types/EDataType.h>
#include <baregl/Buffer.h>

namespace baregl
{
	/**
	* Represents a vertex array, used to describe a vertex layout to the graphics context.
	*/
	class VertexArray final : public detail::NativeObject
	{
	public:
		/**
		* Creates the vertex array.
		*/
		VertexArray();

		/**
		* Destroys the vertex array.
		*/
		~VertexArray();

		/**
		* Returns true if the vertex array is valid (non-empty layout).
		*/
		bool IsValid() const;

		/**
		* Sets the vertex attribute layout.
		* @param p_attributes
		* @param p_vertexBuffer
		* @param p_indexBuffer
		*/
		void SetLayout(
			data::VertexAttributeLayout p_attributes,
			Buffer& p_vertexBuffer,
			Buffer& p_indexBuffer
		);

		/**
		* Resets the vertex attribute layout.
		*/
		void ResetLayout();

		/**
		* Binds the vertex array.
		*/
		void Bind() const;

		/**
		* Unbinds the vertex array.
		*/
		void Unbind() const;

	private:
		uint32_t m_attributeCount = 0;
	};
}
