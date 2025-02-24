/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <memory>

#include "OvRendering/HAL/VertexArray.h"
#include "OvRendering/HAL/IndexBuffer.h"
#include "OvRendering/Resources/IMesh.h"
#include "OvRendering/Geometry/Vertex.h"
#include "OvRendering/Geometry/BoundingSphere.h"

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
		Mesh(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex);

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
		void CreateBuffers(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices);
		void ComputeBoundingSphere(const std::vector<Geometry::Vertex>& p_vertices);

	private:
		const uint32_t m_vertexCount;
		const uint32_t m_indicesCount;
		const uint32_t m_materialIndex;

		HAL::VertexArray m_vertexArray;
		std::unique_ptr<HAL::VertexBuffer> m_vertexBuffer;
		std::unique_ptr<HAL::IndexBuffer> m_indexBuffer;

		Geometry::BoundingSphere m_boundingSphere;
	};
}