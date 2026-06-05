/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <functional>

#include <baregl/Texture.h>
#include <tracy/Tracy.hpp>

#include <OvRendering/Core/ABaseRenderer.h>
#include <OvRendering/Resources/Loaders/TextureLoader.h>

std::atomic_bool OvRendering::Core::ABaseRenderer::s_isDrawing{ false };

namespace
{
	const OvRendering::Entities::Camera kDefaultCamera;

	constexpr auto kUnitQuadVertices = std::to_array<OvRendering::Geometry::Vertex>({
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }, // Bottom-left
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} }, // Bottom-right
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }, // Top-right
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }  // Top-left
	});

	constexpr auto kUnitQuadIndices = std::to_array<uint32_t>({
		0, 1, 2, // First triangle
		0, 2, 3  // Second triangle
	});

	constexpr auto kWhitePixel = std::to_array<uint8_t>({ 255, 255, 255, 255 });
	constexpr auto kBlackPixel = std::to_array<uint8_t, 6 * 4>({ 0 });
}

OvRendering::Core::ABaseRenderer::ABaseRenderer(Context::Driver& p_driver) : 
	m_driver(p_driver),
	m_isDrawing(false),
	m_emptyTexture2D{ baregl::types::ETextureType::TEXTURE_2D },
	m_emptyTextureCube{ baregl::types::ETextureType::TEXTURE_CUBE },
	m_unitQuad(kUnitQuadVertices, kUnitQuadIndices)
{
	const auto kEmptyTextureDesc = baregl::data::TextureDesc{
		.width = 1,
		.height = 1,
		.minFilter = baregl::types::ETextureFilteringMode::NEAREST,
		.magFilter = baregl::types::ETextureFilteringMode::NEAREST,
		.horizontalWrap = baregl::types::ETextureWrapMode::REPEAT,
		.verticalWrap = baregl::types::ETextureWrapMode::REPEAT,
		.internalFormat = baregl::types::EInternalFormat::RGBA8,
		.useMipMaps = false
	};

	m_emptyTexture2D.Allocate(kEmptyTextureDesc);
	m_emptyTexture2D.Upload(kWhitePixel.data(), baregl::types::EFormat::RGBA, baregl::types::EPixelDataType::UNSIGNED_BYTE);

	m_emptyTextureCube.Allocate(kEmptyTextureDesc);
	m_emptyTextureCube.Upload(kBlackPixel.data(), baregl::types::EFormat::RGBA, baregl::types::EPixelDataType::UNSIGNED_BYTE);
}

void OvRendering::Core::ABaseRenderer::BeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
	ZoneScoped;

	OVASSERT(!s_isDrawing, "Cannot call BeginFrame() when previous frame hasn't finished.");
	OVASSERT(p_frameDescriptor.IsValid(), "Invalid FrameDescriptor!");

	m_previousMaterialSignature.reset();

	m_frameDescriptor = p_frameDescriptor;

	if (p_frameDescriptor.outputBuffer)
	{
		p_frameDescriptor.outputBuffer.value().Bind();
	}

	m_basePipelineState = m_driver.CreatePipelineState();
	SetViewport(0, 0, p_frameDescriptor.renderWidth, p_frameDescriptor.renderHeight);

	Clear(
		p_frameDescriptor.camera->GetClearColorBuffer(),
		p_frameDescriptor.camera->GetClearDepthBuffer(),
		p_frameDescriptor.camera->GetClearStencilBuffer(),
		{ p_frameDescriptor.camera.value().GetClearColor(), 1.0f }
	);

	p_frameDescriptor.camera->CacheMatrices(p_frameDescriptor.renderWidth, p_frameDescriptor.renderHeight);

	m_isDrawing = true;
	s_isDrawing.store(true);
}

void OvRendering::Core::ABaseRenderer::EndFrame()
{
	ZoneScoped;

	OVASSERT(s_isDrawing, "Cannot call EndFrame() before calling BeginFrame()");

	if (m_frameDescriptor.outputBuffer)
	{
		m_frameDescriptor.outputBuffer.value().Unbind();
	}

	m_isDrawing = false;
	s_isDrawing.store(false);
}

const OvRendering::Data::FrameDescriptor& OvRendering::Core::ABaseRenderer::GetFrameDescriptor() const
{
	OVASSERT(m_isDrawing, "Cannot call GetFrameDescriptor() outside of a frame");
	return m_frameDescriptor;
}

OvRendering::Data::PipelineState OvRendering::Core::ABaseRenderer::CreatePipelineState() const
{
	return m_basePipelineState;
}

bool OvRendering::Core::ABaseRenderer::IsDrawing() const
{
	return m_isDrawing;
}

void OvRendering::Core::ABaseRenderer::SetViewport(uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height)
{
	m_driver.SetViewport(p_x, p_y, p_width, p_height);
}

void OvRendering::Core::ABaseRenderer::Clear(
	bool p_colorBuffer,
	bool p_depthBuffer,
	bool p_stencilBuffer,
	const OvMaths::FVector4& p_color
)
{
	ZoneScoped;
	m_driver.Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer, p_color);
}

void OvRendering::Core::ABaseRenderer::Blit(
	OvRendering::Data::PipelineState p_pso,
	baregl::Framebuffer& p_src,
	baregl::Framebuffer& p_dst,
	OvRendering::Data::Material& p_material,
	OvRendering::Settings::EBlitFlags p_flags
)
{
	ZoneScoped;

	const auto [srcWidth, srcHeight] = p_src.GetSize();

	if (OvRendering::Settings::IsFlagSet(OvRendering::Settings::EBlitFlags::RESIZE_DST_TO_MATCH_SRC, p_flags))
	{
		p_dst.Resize(srcWidth, srcHeight);
	}

	if (OvRendering::Settings::IsFlagSet(OvRendering::Settings::EBlitFlags::FILL_INPUT_TEXTURE, p_flags))
	{
		const auto colorTex = p_src.GetAttachment<baregl::Texture>(baregl::types::EFramebufferAttachment::COLOR);
		OVASSERT(colorTex.has_value(), "Invalid color attachment");
		p_material.SetProperty("_InputTexture", &colorTex.value().get());
	}

	OvRendering::Entities::Drawable blit;
	blit.mesh = m_unitQuad;
	blit.material = p_material;

	if (OvRendering::Settings::IsFlagSet(OvRendering::Settings::EBlitFlags::USE_MATERIAL_STATE_MASK, p_flags))
	{
		blit.stateMask = p_material.GenerateStateMask();
	}
	else
	{
		blit.stateMask.depthWriting = false;
		blit.stateMask.colorWriting = true;
		blit.stateMask.blendable = false;
		blit.stateMask.frontfaceCulling = false;
		blit.stateMask.backfaceCulling = false;
		blit.stateMask.depthTest = false;
	}

	p_dst.Bind();

	if (OvRendering::Settings::IsFlagSet(OvRendering::Settings::EBlitFlags::UPDATE_VIEWPORT_SIZE, p_flags))
	{
		const auto [dstWidth, dstHeight] = p_dst.GetSize();
		SetViewport(0, 0, dstWidth, dstHeight);
	}

	DrawEntity(p_pso, blit);
	p_dst.Unbind();
}

bool OvRendering::Core::ABaseRenderer::IsDrawable(const Entities::Drawable& p_drawable) const
{
	return
		p_drawable.mesh &&
		p_drawable.material &&
		p_drawable.material->IsValid()&&
		p_drawable.material->SupportsProjectionMode(m_frameDescriptor.camera->GetProjectionMode()) &&
		p_drawable.material->GetGPUInstances() > 0;
}

void OvRendering::Core::ABaseRenderer::DrawEntity(
	OvRendering::Data::PipelineState p_pso,
	const Entities::Drawable& p_drawable
)
{
	ZoneScoped;

	OVASSERT(IsDrawable(p_drawable), "Submitted an entity that isn't properly configured!");

	p_pso.depthWriting = p_drawable.stateMask.depthWriting;
	p_pso.colorWriting.mask = p_drawable.stateMask.colorWriting ? 0xFF : 0x00;
	p_pso.blending = p_drawable.stateMask.blendable;
	p_pso.culling = p_drawable.stateMask.frontfaceCulling || p_drawable.stateMask.backfaceCulling;
	p_pso.depthTest = p_drawable.stateMask.depthTest;

	if (p_pso.culling)
	{
		if (p_drawable.stateMask.backfaceCulling && p_drawable.stateMask.frontfaceCulling)
		{
			p_pso.cullFace = baregl::types::ECullFace::FRONT_AND_BACK;
		}
		else
		{
			p_pso.cullFace =
				p_drawable.stateMask.backfaceCulling ?
				baregl::types::ECullFace::BACK :
				baregl::types::ECullFace::FRONT;
		}
	}

	const auto signature = p_drawable.material->Bind(
		p_drawable.pass,
		p_drawable.featureSetOverride.has_value() ?
			OvTools::Utils::OptRef<const Data::FeatureSet>(p_drawable.featureSetOverride.value()) :
			std::nullopt,
		&m_emptyTexture2D,
		&m_emptyTextureCube,
		m_previousMaterialSignature
	);

	const bool uploadStableProperties = !m_previousMaterialSignature.has_value() || signature.stablePropertySignature != m_previousMaterialSignature->stablePropertySignature;
	const bool uploadSingleUseProperties = !m_previousMaterialSignature.has_value() || signature.singleUsePropertySignature != m_previousMaterialSignature->singleUsePropertySignature;

	if (uploadStableProperties || uploadSingleUseProperties)
	{
		p_drawable.material->UploadProperties(
			uploadStableProperties,
			uploadSingleUseProperties,
			&m_emptyTexture2D,
			&m_emptyTextureCube
		);
	}

	m_previousMaterialSignature = signature;

	m_driver.Draw(
		p_pso,
		p_drawable.mesh.value(),
		p_drawable.primitiveMode,
		p_drawable.material->GetGPUInstances()
	);

	p_drawable.material->Unbind();
}
