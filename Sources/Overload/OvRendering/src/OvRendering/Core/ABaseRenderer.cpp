/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <functional>

#include "OvRendering/Core/ABaseRenderer.h"
#include "OvRendering/Resources/Loaders/TextureLoader.h"
#include <OvRendering/HAL/TextureHandle.h>

std::atomic_bool OvRendering::Core::ABaseRenderer::s_isDrawing{ false };

const OvRendering::Entities::Camera kDefaultCamera;

std::unique_ptr<OvRendering::Resources::Mesh> CreateUnitQuad()
{
	const std::vector<OvRendering::Geometry::Vertex> vertices = {
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }, // Bottom-left
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} }, // Bottom-right
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }, // Top-right
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }  // Top-left
	};

	const std::vector<uint32_t> indices = {
		0, 1, 2, // First triangle
		0, 2, 3  // Second triangle
	};

	return std::make_unique<OvRendering::Resources::Mesh>(vertices, indices, 0);
}

OvRendering::Core::ABaseRenderer::ABaseRenderer(Context::Driver& p_driver) : 
	m_driver(p_driver),
	m_isDrawing(false),
	m_emptyTexture(OvRendering::Resources::Loaders::TextureLoader::CreatePixel(255, 255, 255, 255)),
	m_unitQuad(CreateUnitQuad())
{
}

OvRendering::Core::ABaseRenderer::~ABaseRenderer()
{
	OvRendering::Resources::Loaders::TextureLoader::Destroy(m_emptyTexture);
}

void OvRendering::Core::ABaseRenderer::BeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
	OVASSERT(!s_isDrawing, "Cannot call BeginFrame() when previous frame hasn't finished.");
	OVASSERT(p_frameDescriptor.IsValid(), "Invalid FrameDescriptor!");

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
	m_driver.Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer, p_color);
}

void OvRendering::Core::ABaseRenderer::Blit(
	OvRendering::Data::PipelineState p_pso,
	OvRendering::HAL::Framebuffer& p_src,
	OvRendering::HAL::Framebuffer& p_dst,
	OvRendering::Data::Material& p_material,
	OvRendering::Settings::EBlitFlags p_flags
)
{
	OVASSERT(m_unitQuad != nullptr, "Invalid unit quad mesh, cannot blit!");

	auto [srcWidth, srcHeight] = p_src.GetSize();

	if (OvRendering::Settings::IsFlagSet(OvRendering::Settings::EBlitFlags::RESIZE_DST_TO_MATCH_SRC, p_flags))
	{
		p_dst.Resize(srcWidth, srcHeight);
	}

	if (OvRendering::Settings::IsFlagSet(OvRendering::Settings::EBlitFlags::FILL_INPUT_TEXTURE, p_flags))
	{
		const auto colorTex = p_src.GetAttachment<HAL::Texture>(Settings::EFramebufferAttachment::COLOR);
		OVASSERT(colorTex.has_value(), "Invalid color attachment");
		p_material.Set("_InputTexture", colorTex);
	}

	OvRendering::Entities::Drawable blit;
	blit.mesh = *m_unitQuad;
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
		SetViewport(0, 0, srcWidth, srcHeight);
	}

	DrawEntity(p_pso, blit);
	p_dst.Unbind();
}

void OvRendering::Core::ABaseRenderer::DrawEntity(
	OvRendering::Data::PipelineState p_pso,
	const Entities::Drawable& p_drawable
)
{
	auto material = p_drawable.material;
	auto mesh = p_drawable.mesh;

	const auto gpuInstances = material.value().GetGPUInstances();

	if (mesh && material && material->IsValid() && gpuInstances > 0)
	{
		p_pso.depthWriting = p_drawable.stateMask.depthWriting;
		p_pso.colorWriting.mask = p_drawable.stateMask.colorWriting ? 0xFF : 0x00;
		p_pso.blending = p_drawable.stateMask.blendable;
		p_pso.culling = p_drawable.stateMask.frontfaceCulling || p_drawable.stateMask.backfaceCulling;
		p_pso.depthTest = p_drawable.stateMask.depthTest;

		if (p_pso.culling)
		{
			if (p_drawable.stateMask.backfaceCulling && p_drawable.stateMask.frontfaceCulling)
			{
				p_pso.cullFace = Settings::ECullFace::FRONT_AND_BACK;
			}
			else
			{
				p_pso.cullFace =
					p_drawable.stateMask.backfaceCulling ?
					Settings::ECullFace::BACK :
					Settings::ECullFace::FRONT;
			}
		}

		material->Bind(m_emptyTexture);
		m_driver.Draw(p_pso, mesh.value(), p_drawable.primitiveMode, gpuInstances);
		material->UnBind();
	}
}
