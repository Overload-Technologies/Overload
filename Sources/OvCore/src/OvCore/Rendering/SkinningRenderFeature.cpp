/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <string>

#include <OvCore/Rendering/SkinningDrawableDescriptor.h>
#include <OvCore/Rendering/SkinningRenderFeature.h>
#include <OvCore/Rendering/SkinningUtils.h>

namespace
{
	const std::string kSkinningFeatureName{ OvCore::Rendering::SkinningUtils::kFeatureName };
}

OvCore::Rendering::SkinningRenderFeature::SkinningRenderFeature(
	OvRendering::Core::CompositeRenderer& p_renderer,
	OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy,
	uint32_t p_bufferBindingPoint
) :
	ARenderFeature(p_renderer, p_executionPolicy),
	m_bufferBindingPoint(p_bufferBindingPoint)
{
	for (auto& skinningBuffer : m_skinningBuffers)
	{
		skinningBuffer = std::make_unique<OvRendering::HAL::ShaderStorageBuffer>();
	}

	m_identityBuffer = std::make_unique<OvRendering::HAL::ShaderStorageBuffer>();
}

uint32_t OvCore::Rendering::SkinningRenderFeature::GetBufferBindingPoint() const
{
	return m_bufferBindingPoint;
}

void OvCore::Rendering::SkinningRenderFeature::OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor)
{
	(void)p_frameDescriptor;
	m_skinningBufferIndex = (m_skinningBufferIndex + 1) % kSkinningBufferRingSize;

	m_lastUploaded = {};
	m_bound = {};

	if (m_identityBuffer->GetSize() == 0)
	{
		const auto identity = OvMaths::FMatrix4::Transpose(OvMaths::FMatrix4::Identity);
		m_identityBuffer->Allocate(sizeof(OvMaths::FMatrix4), OvRendering::Settings::EAccessSpecifier::STATIC_DRAW);
		m_identityBuffer->Upload(&identity);
	}
}

void OvCore::Rendering::SkinningRenderFeature::OnEndFrame()
{
	GetCurrentSkinningBuffer().Unbind();
	m_identityBuffer->Unbind();
	m_bound = {};
}

void OvCore::Rendering::SkinningRenderFeature::OnBeforeDraw(
	OvRendering::Data::PipelineState& p_pso,
	const OvRendering::Entities::Drawable& p_drawable
)
{
	(void)p_pso;

	OvTools::Utils::OptRef<const SkinningDrawableDescriptor> skinningDescriptor;
	const bool hasSkinningDescriptor = p_drawable.TryGetDescriptor<SkinningDrawableDescriptor>(skinningDescriptor);

	const bool usesSkinningVariant =
		(p_drawable.featureSetOverride.has_value() && p_drawable.featureSetOverride->contains(kSkinningFeatureName)) ||
		(p_drawable.material.has_value() && p_drawable.material->GetFeatures().contains(kSkinningFeatureName));

	if (!usesSkinningVariant)
	{
		return;
	}

	if (!hasSkinningDescriptor ||
		!skinningDescriptor->matrices ||
		skinningDescriptor->count == 0)
	{
		BindIdentityPalette();
		return;
	}

	const bool mustUpload =
		m_lastUploaded.ptr != skinningDescriptor->matrices ||
		m_lastUploaded.count != skinningDescriptor->count ||
		m_lastUploaded.poseVersion != skinningDescriptor->poseVersion;

	auto& skinningBuffer = GetCurrentSkinningBuffer();

	if (mustUpload)
	{
		const auto uploadSize = static_cast<uint64_t>(skinningDescriptor->count) * sizeof(OvMaths::FMatrix4);
		if (skinningBuffer.GetSize() < uploadSize)
		{
			skinningBuffer.Allocate(uploadSize, OvRendering::Settings::EAccessSpecifier::STREAM_DRAW);
		}

		skinningBuffer.Upload(skinningDescriptor->matrices, OvRendering::HAL::BufferMemoryRange{
			.offset = 0,
			.size = uploadSize
		});

		m_lastUploaded = { skinningDescriptor->matrices, skinningDescriptor->count, skinningDescriptor->poseVersion };
	}

	const bool mustBindSkinningPalette =
		m_bound.type != EBoundPalette::SKINNING ||
		m_bound.ptr != skinningDescriptor->matrices ||
		m_bound.count != skinningDescriptor->count ||
		m_bound.poseVersion != skinningDescriptor->poseVersion;

	if (mustBindSkinningPalette)
	{
		skinningBuffer.Bind(m_bufferBindingPoint);
		m_bound = { EBoundPalette::SKINNING, skinningDescriptor->matrices, skinningDescriptor->count, skinningDescriptor->poseVersion };
	}
}

OvRendering::HAL::ShaderStorageBuffer& OvCore::Rendering::SkinningRenderFeature::GetCurrentSkinningBuffer() const
{
	return *m_skinningBuffers[m_skinningBufferIndex];
}

void OvCore::Rendering::SkinningRenderFeature::BindIdentityPalette() const
{
	if (m_bound.type != EBoundPalette::IDENTITY)
	{
		m_identityBuffer->Bind(m_bufferBindingPoint);
		m_bound = { EBoundPalette::IDENTITY };
	}
}
