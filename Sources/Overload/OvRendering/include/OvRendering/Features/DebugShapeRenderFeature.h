/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvRendering/Features/ARenderFeature.h"

namespace OvRendering::Features
{
	/**
	* The ShapeDrawer handles the drawing of basic shapes
	*/
	class DebugShapeRenderFeature : public ARenderFeature
	{
	public:
		/**
		* Constructor
		* @param p_renderer
		*/
		DebugShapeRenderFeature(Core::CompositeRenderer& p_renderer);

		/**
		* Destructor
		*/
		virtual ~DebugShapeRenderFeature();

		/**
		* Draw a line in world space
		* @param p_pso
		* @param p_start
		* @param p_end
		* @param p_color
		* @param p_lineWidth
		*/
		void DrawLine(
			OvRendering::Data::PipelineState p_pso,
			const OvMaths::FVector3& p_start,
			const OvMaths::FVector3& p_end,
			const OvMaths::FVector3& p_color,
			float p_lineWidth = 1.0f
		);

		/**
		* Draw a box in world space
		* @param p_pso
		* @param p_position
		* @param p_rotation
		* @param p_size
		* @param p_color
		* @param p_lineWidth
		*/
		void DrawBox(
			OvRendering::Data::PipelineState p_pso,
			const OvMaths::FVector3& p_position,
			const OvMaths::FQuaternion& p_rotation,
			const OvMaths::FVector3& p_size,
			const OvMaths::FVector3& p_color,
			float p_lineWidth = 1.0f
		);

		/**
		* Draw a sphere in world space
		* @param p_pso
		* @param p_position
		* @param p_rotation
		* @param p_radius
		* @param p_color
		* @param p_lineWidth
		*/
		void DrawSphere(
			OvRendering::Data::PipelineState p_pso,
			const OvMaths::FVector3& p_position,
			const OvMaths::FQuaternion& p_rotation,
			float p_radius,
			const OvMaths::FVector3& p_color,
			float p_lineWidth = 1.0f
		);

		/**
		* Draw a capsule in world space
		* @param p_pso
		* @param p_position
		* @param p_rotation
		* @param p_radius
		* @param p_height
		* @param p_color
		* @param p_lineWidth
		*/
		void DrawCapsule(
			OvRendering::Data::PipelineState p_pso,
			const OvMaths::FVector3& p_position,
			const OvMaths::FQuaternion& p_rotation,
			float p_radius,
			float p_height,
			const OvMaths::FVector3& p_color,
			float p_lineWidth = 1.0f
		);

	protected:
		virtual void OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor) override;

	private:
		OvRendering::Resources::Shader* m_lineShader = nullptr;

		std::unique_ptr<OvRendering::Resources::Mesh> m_lineMesh;
		std::unique_ptr<OvRendering::Data::Material> m_lineMaterial;
	};
}