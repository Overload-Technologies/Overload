/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <OvRendering/HAL/IndexBuffer.h>
#include <OvRendering/HAL/VertexArray.h>
#include <OvRendering/HAL/VertexBuffer.h>
#include <OvRendering/Geometry/Vertex.h>
#include <OvRendering/Geometry/BoundingSphere.h>
#include <OvRendering/Resources/IMesh.h>

namespace OvRendering::Resources
{
	/**
	* Standard mesh of OvRendering
	*/
	class Mesh : public IMesh
	{
	public:
		/**
		* Create a mesh with the given vertices, indices and material index
		* @param p_vertices
		* @param p_indices
		* @param p_materialIndex
		*/
		Mesh(
			std::span<const Geometry::Vertex> p_vertices,
			std::span<const uint32_t> p_indices,
			uint32_t p_materialIndex = 0
		);

		/**
		* Bind the mesh (Actually bind its VAO)
		*/
		virtual void Bind() const override;

		/**
		* Unbind the mesh (Actually unbind its VAO)
		*/
		virtual void Unbind() const override;

		/**
		* Returns the number of vertices
		*/
		virtual uint32_t GetVertexCount() const override;

		/**
		* Returns the number of indices
		*/
		virtual uint32_t GetIndexCount() const override;

		/**
		* Returns the material index of the mesh
		*/
		uint32_t GetMaterialIndex() const;

		/**
		* Returns the bounding sphere of the mesh
		*/
		const OvRendering::Geometry::BoundingSphere& GetBoundingSphere() const;

	private:
		void Upload(std::span<const Geometry::Vertex> p_vertices, std::span<const uint32_t> p_indices);
		void ComputeBoundingSphere(std::span<const Geometry::Vertex> p_vertices);

	private:
		const uint32_t m_vertexCount;
		const uint32_t m_indicesCount;
		const uint32_t m_materialIndex;

		HAL::VertexArray m_vertexArray;
		HAL::VertexBuffer m_vertexBuffer;
		HAL::IndexBuffer m_indexBuffer;

		Geometry::BoundingSphere m_boundingSphere;
	};
}