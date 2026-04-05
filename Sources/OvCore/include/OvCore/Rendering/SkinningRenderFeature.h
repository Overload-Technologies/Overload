/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include <OvMaths/FMatrix4.h>
#include <OvRendering/Features/ARenderFeature.h>
#include <OvRendering/HAL/ShaderStorageBuffer.h>

namespace OvCore::Rendering
{
	/**
	* Render feature responsible for uploading and binding skinning matrices.
	*/
	class SkinningRenderFeature : public OvRendering::Features::ARenderFeature
	{
	public:
		static constexpr uint32_t kDefaultBufferBindingPoint = 1;

		/**
		* Constructor
		* @param p_renderer
		* @param p_executionPolicy
		* @param p_bufferBindingPoint
		*/
		SkinningRenderFeature(
			OvRendering::Core::CompositeRenderer& p_renderer,
			OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy,
			uint32_t p_bufferBindingPoint = kDefaultBufferBindingPoint
		);

		/**
		* Returns the skinning buffer binding point
		*/
		uint32_t GetBufferBindingPoint() const;

	protected:
		virtual void OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor) override;
		virtual void OnEndFrame() override;
		virtual void OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable) override;

	private:
		void BindIdentityPalette() const;
		OvRendering::HAL::ShaderStorageBuffer& GetCurrentSkinningBuffer() const;

	private:
		static constexpr uint32_t kSkinningBufferRingSize = 3;

		enum class EBoundPalette
		{
			NONE,
			IDENTITY,
			SKINNING
		};

		struct UploadedPaletteState
		{
			const OvMaths::FMatrix4* ptr = nullptr;
			uint32_t count = 0;
			uint64_t poseVersion = 0;
		};

		struct BoundPaletteState
		{
			EBoundPalette type = EBoundPalette::NONE;
			const OvMaths::FMatrix4* ptr = nullptr;
			uint32_t count = 0;
			uint64_t poseVersion = 0;
		};

		uint32_t m_bufferBindingPoint;
		mutable uint32_t m_skinningBufferIndex = kSkinningBufferRingSize - 1;
		std::array<std::unique_ptr<OvRendering::HAL::ShaderStorageBuffer>, kSkinningBufferRingSize> m_skinningBuffers;
		std::unique_ptr<OvRendering::HAL::ShaderStorageBuffer> m_identityBuffer;

		UploadedPaletteState m_lastUploaded;
		mutable BoundPaletteState m_bound;
	};
}
