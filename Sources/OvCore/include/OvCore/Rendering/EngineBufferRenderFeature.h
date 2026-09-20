/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <chrono>
#include <cstddef>
#include <memory>

#include <baregl/Buffer.h>

#include <OvRendering/Features/ARenderFeature.h>
#include <OvRendering/Entities/Camera.h>

namespace OvCore::Rendering
{
	/**
	* Render feature handling engine buffer (UBO) updates
	*/
	class EngineBufferRenderFeature : public OvRendering::Features::ARenderFeature
	{
	public:
		/**
		* Constructor
		* @param p_renderer
		* @param p_executionPolicy
		*/
		EngineBufferRenderFeature(
			OvRendering::Core::CompositeRenderer& p_renderer,
			OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
		);

		/**
		* Replace the current camera data in the engine buffer by the provided camera
		* @param p_camera
		*/
		void SetCamera(const OvRendering::Entities::Camera& p_camera);

	protected:
		virtual void OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor) override;
		virtual void OnEndFrame() override;
		virtual void OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable) override;

	private:
		struct CameraData
		{
			OvMaths::FMatrix4 viewMatrix;
			OvMaths::FMatrix4 projectionMatrix;
			OvMaths::FVector3 position;
		};
		static_assert(
			sizeof(CameraData) == sizeof(OvMaths::FMatrix4) * 2 + sizeof(OvMaths::FVector3),
			"CameraData must match the engine UBO camera page"
		);

		void UploadCameraData(const CameraData& p_cameraData);
		void RestoreFrameCamera();

	protected:
		std::chrono::high_resolution_clock::time_point m_startTime;
		std::unique_ptr<baregl::Buffer> m_engineBuffer;
		CameraData m_frameCameraData;
		bool m_cameraOverrideActive = false;
	};
}
