/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>

#include "OvRendering/Context/Driver.h"
#include "OvRendering/Utils/Conversions.h"

#include "OvRendering/HAL/Backend.h"

std::unique_ptr<OvRendering::HAL::Backend> m_gfxBackend;

OvRendering::Context::Driver::Driver(const OvRendering::Settings::DriverSettings& p_driverSettings)
{
	m_gfxBackend = std::make_unique<OvRendering::HAL::Backend>();

	auto initialPipelineState = m_gfxBackend->Init(p_driverSettings.debugMode);

	OVASSERT(initialPipelineState.has_value(), "Failed to initialized driver!");

	if (p_driverSettings.defaultPipelineState)
	{
		m_defaultPipelineState = p_driverSettings.defaultPipelineState.value();
	}
	
	m_pipelineState = initialPipelineState.value();
	SetPipelineState(m_defaultPipelineState);

	m_vendor = m_gfxBackend->GetVendor();
	m_hardware = m_gfxBackend->GetHardware();
	m_version = m_gfxBackend->GetVersion();
	m_shadingLanguageVersion = m_gfxBackend->GetShadingLanguageVersion();

	OVLOG_INFO("Graphics driver initialized:");
	OVLOG_INFO("\tVendor => " + m_vendor);
	OVLOG_INFO("\tHardware => " + m_hardware);
	OVLOG_INFO("\tVersion => " + m_version);
	OVLOG_INFO("\tShading Language => " + m_shadingLanguageVersion);
}

OvRendering::Context::Driver::~Driver()
{
}

void OvRendering::Context::Driver::SetViewport(uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height)
{
	m_gfxBackend->SetViewport(p_x, p_y, p_width, p_height);
}

void OvRendering::Context::Driver::Clear(
	bool p_colorBuffer,
	bool p_depthBuffer,
	bool p_stencilBuffer,
	const OvMaths::FVector4& p_color
)
{
	if (p_colorBuffer)
	{
		m_gfxBackend->SetClearColor(p_color.x, p_color.y, p_color.z, p_color.w);
	}

	auto pso = CreatePipelineState();

	if (p_stencilBuffer)
	{
		pso.stencilWriteMask = ~0;
	}

	pso.scissorTest = false;

	SetPipelineState(pso);

	m_gfxBackend->Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer);
}

void OvRendering::Context::Driver::Draw(
	Data::PipelineState p_pso,
	const Resources::IMesh& p_mesh,
	Settings::EPrimitiveMode p_primitiveMode,
	uint32_t p_instances
)
{
	if (p_instances > 0)
	{
		SetPipelineState(p_pso);

		p_mesh.Bind();

		if (p_mesh.GetIndexCount() > 0)
		{
			if (p_instances == 1)
			{
				m_gfxBackend->DrawElements(p_primitiveMode, p_mesh.GetIndexCount());
			}
			else
			{
				m_gfxBackend->DrawElementsInstanced(p_primitiveMode, p_mesh.GetIndexCount(), p_instances);
			}
		}
		else
		{
			if (p_instances == 1)
			{
				m_gfxBackend->DrawArrays(p_primitiveMode, p_mesh.GetVertexCount());
			}
			else
			{
				m_gfxBackend->DrawArraysInstanced(p_primitiveMode, p_mesh.GetVertexCount(), p_instances);
			}
		}

		p_mesh.Unbind();
	}
}

void OvRendering::Context::Driver::SetPipelineState(OvRendering::Data::PipelineState p_state)
{
	using namespace OvRendering::Settings;

	if (p_state.bits != m_pipelineState.bits)
	{
		auto& i = p_state;
		auto& c = m_pipelineState;

		// Rasterization
		if (i.rasterizationMode != c.rasterizationMode) m_gfxBackend->SetRasterizationMode(i.rasterizationMode);
		if (i.lineWidthPow2 != c.lineWidthPow2) m_gfxBackend->SetRasterizationLinesWidth(Utils::Conversions::Pow2toFloat(i.lineWidthPow2));

		if (i.colorWriting.mask != c.colorWriting.mask) m_gfxBackend->SetColorWriting(i.colorWriting.r, i.colorWriting.g, i.colorWriting.b, i.colorWriting.a);
		if (i.depthWriting != c.depthWriting) m_gfxBackend->SetDepthWriting(i.depthWriting);

		if (i.blending != c.blending) m_gfxBackend->SetCapability(ERenderingCapability::BLEND, i.blending);
		if (i.culling != c.culling) m_gfxBackend->SetCapability(ERenderingCapability::CULL_FACE, i.culling);
		if (i.dither != c.dither) m_gfxBackend->SetCapability(ERenderingCapability::DITHER, i.dither);
		if (i.polygonOffsetFill != c.polygonOffsetFill) m_gfxBackend->SetCapability(ERenderingCapability::POLYGON_OFFSET_FILL, i.polygonOffsetFill);
		if (i.sampleAlphaToCoverage != c.sampleAlphaToCoverage) m_gfxBackend->SetCapability(ERenderingCapability::SAMPLE_ALPHA_TO_COVERAGE, i.sampleAlphaToCoverage);
		if (i.depthTest != c.depthTest) m_gfxBackend->SetCapability(ERenderingCapability::DEPTH_TEST, i.depthTest);
		if (i.scissorTest != c.scissorTest) m_gfxBackend->SetCapability(ERenderingCapability::SCISSOR_TEST, i.scissorTest);
		if (i.stencilTest != c.stencilTest) m_gfxBackend->SetCapability(ERenderingCapability::STENCIL_TEST, i.stencilTest);
		if (i.multisample != c.multisample) m_gfxBackend->SetCapability(ERenderingCapability::MULTISAMPLE, i.multisample);

		// Stencil algorithm
		if (i.stencilFuncOp != c.stencilFuncOp ||
			i.stencilFuncRef != c.stencilFuncRef ||
			i.stencilFuncMask != c.stencilFuncMask)
			m_gfxBackend->SetStencilAlgorithm(i.stencilFuncOp, i.stencilFuncRef, i.stencilFuncMask);

		if (i.stencilWriteMask != c.stencilWriteMask) m_gfxBackend->SetStencilMask(i.stencilWriteMask);
		if (i.stencilOpFail != c.stencilOpFail || i.depthOpFail != c.depthOpFail || i.bothOpFail != c.bothOpFail) m_gfxBackend->SetStencilOperations(i.stencilOpFail, i.depthOpFail, i.bothOpFail);

		// Depth
		if (i.depthFunc != c.depthFunc) m_gfxBackend->SetDepthAlgorithm(i.depthFunc);

		// Culling
		if (i.cullFace != c.cullFace) m_gfxBackend->SetCullFace(i.cullFace);

		m_pipelineState = p_state;
	}
}

void OvRendering::Context::Driver::ResetPipelineState()
{
	SetPipelineState(m_defaultPipelineState);
}

OvRendering::Data::PipelineState OvRendering::Context::Driver::CreatePipelineState() const
{
	return m_defaultPipelineState;
}

std::string_view OvRendering::Context::Driver::GetVendor() const
{
	return m_vendor;
}

std::string_view OvRendering::Context::Driver::GetHardware() const
{
	return m_hardware;
}

std::string_view OvRendering::Context::Driver::GetVersion() const
{
	return m_version;
}

std::string_view OvRendering::Context::Driver::GetShadingLanguageVersion() const
{
	return m_shadingLanguageVersion;
}
