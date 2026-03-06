/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TVertexBuffer.h>
#include <OvRendering/HAL/Common/TIndexBuffer.h>
#include <OvRendering/Settings/EDataType.h>
#include <OvRendering/Settings/EGraphicsBackend.h>
#include <OvRendering/Settings/VertexAttribute.h>

namespace OvRendering::HAL
{
	/**
	* Represents a vertex array, used to descript the vertex layout to the graphics backend.
	*/
	template<Settings::EGraphicsBackend Backend, class VertexArrayContext, class VertexBufferContext, class IndexBufferContext, class BufferContext>
	class TVertexArray final
	{
	public:
		using IndexBuffer = TIndexBuffer<Backend, IndexBufferContext, BufferContext>;
		using VertexBuffer = TVertexBuffer<Backend, VertexBufferContext, BufferContext>;

		/**
		* Creates the vertex array.
		*/
		TVertexArray();

		/**
		* Destroys the vertex array.
		*/
		~TVertexArray();

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
			Settings::VertexAttributeLayout p_attributes,
			VertexBuffer& p_vertexBuffer,
			IndexBuffer& p_indexBuffer
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

		/**
		* Returns the vertex array ID.
		*/
		uint32_t GetID() const;

	private:
		VertexArrayContext m_context;
	};
}
