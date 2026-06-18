/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>

#include <baregl/debug/Debug.h>
#include <tracy/Tracy.hpp>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>

#include <OvRendering/Context/Driver.h>
#include <OvRendering/Utils/Conversions.h>
#include <OvRendering/Utils/Profiling.h>

namespace
{
	class AssertHandler : public baregl::debug::IAssertHandler
	{
		void OnAssert(const std::string_view p_message) override { OVASSERT(false, p_message); }
	};

	class LogHandler : public baregl::debug::ILogHandler
	{
		void LogInfo(const std::string_view p_message) override { OVLOG_INFO(std::string{p_message}); }
		void LogWarning(const std::string_view p_message) override { OVLOG_WARNING(std::string{p_message}); }
		void LogError(const std::string_view p_message) override { OVLOG_ERROR(std::string{p_message}); }
	};

	/**
	* Very expensive! Call it once, and make sure you always keep track of state changes
	*/
	OvRendering::Data::PipelineState RetrieveOpenGLPipelineState(baregl::Context &p_ctx)
	{
		using namespace OvRendering::Settings;
		using namespace baregl::types;
		using namespace baregl::utils;

		OvRendering::Data::PipelineState pso;

		// Rasterization
		pso.rasterizationMode = p_ctx.Get<EGetParameter::POLYGON_MODE>();
		pso.lineWidthPow2 = OvRendering::Utils::Conversions::FloatToPow2(
				p_ctx.Get<EGetParameter::LINE_WIDTH>());

		// Color write mask
		{
			std::array<bool, 4> colorWriteMask =
					p_ctx.Get<EGetParameter::COLOR_WRITEMASK>();
			pso.colorWriting.r = colorWriteMask[0];
			pso.colorWriting.g = colorWriteMask[1];
			pso.colorWriting.b = colorWriteMask[2];
			pso.colorWriting.a = colorWriteMask[3];
		}

		// Capabilities
		pso.depthWriting = p_ctx.Get<EGetParameter::DEPTH_WRITEMASK>();
		pso.blending = p_ctx.GetCapability(ERenderingCapability::BLEND);
		pso.culling = p_ctx.GetCapability(ERenderingCapability::CULL_FACE);
		pso.dither = p_ctx.GetCapability(ERenderingCapability::DITHER);
		pso.polygonOffsetFill = p_ctx.GetCapability(ERenderingCapability::POLYGON_OFFSET_FILL);
		pso.sampleAlphaToCoverage = p_ctx.GetCapability(ERenderingCapability::SAMPLE_ALPHA_TO_COVERAGE);
		pso.depthTest = p_ctx.GetCapability(ERenderingCapability::DEPTH_TEST);
		pso.scissorTest = p_ctx.GetCapability(ERenderingCapability::SCISSOR_TEST);
		pso.stencilTest = p_ctx.GetCapability(ERenderingCapability::STENCIL_TEST);
		pso.multisample = p_ctx.GetCapability(ERenderingCapability::MULTISAMPLE);

		// Stencil
		pso.stencilFuncOp = p_ctx.Get<EGetParameter::STENCIL_FUNC>();
		pso.stencilFuncRef = p_ctx.Get<EGetParameter::STENCIL_REF>();
		pso.stencilFuncMask = static_cast<uint32_t>(p_ctx.Get<EGetParameter::STENCIL_VALUE_MASK>());

		pso.stencilWriteMask = static_cast<uint32_t>(p_ctx.Get<EGetParameter::STENCIL_WRITEMASK>());

		pso.stencilOpFail = p_ctx.Get<EGetParameter::STENCIL_FAIL>();
		pso.depthOpFail = p_ctx.Get<EGetParameter::STENCIL_PASS_DEPTH_FAIL>();
		pso.bothOpFail = p_ctx.Get<EGetParameter::STENCIL_PASS_DEPTH_PASS>();

		// Depth
		pso.depthFunc = p_ctx.Get<EGetParameter::DEPTH_FUNC>();

		// Culling
		pso.cullFace = p_ctx.Get<EGetParameter::CULL_FACE_MODE>();

		// Blending
		pso.blendingSrcFactor = p_ctx.Get<EGetParameter::BLEND_SRC_RGB>();
		pso.blendingDestFactor = p_ctx.Get<EGetParameter::BLEND_DST_RGB>();
		pso.blendingEquation = p_ctx.Get<EGetParameter::BLEND_EQUATION_RGB>();

		return pso;
	}
} // namespace

OvRendering::Context::Driver::Driver(const OvRendering::Settings::DriverSettings& p_driverSettings)
{
	baregl::debug::SetAssertHandler(std::make_unique<AssertHandler>());
	baregl::debug::SetLogHandler(std::make_unique<LogHandler>());

	m_gfxContext = std::make_unique<baregl::Context>(baregl::data::ContextDesc{
		p_driverSettings.debugMode
	});

	TracyGpuContext;
	
	auto initialPipelineState = RetrieveOpenGLPipelineState(*m_gfxContext);

	if (p_driverSettings.defaultPipelineState)
	{
		m_defaultPipelineState = p_driverSettings.defaultPipelineState.value();
	}
	
	m_pipelineState = initialPipelineState;
	SetPipelineState(m_defaultPipelineState);

	m_vendor = m_gfxContext->Get<baregl::types::EGetParameter::VENDOR>();
	m_hardware = m_gfxContext->Get<baregl::types::EGetParameter::RENDERER>();
	m_version = m_gfxContext->Get<baregl::types::EGetParameter::VERSION>();
	m_shadingLanguageVersion = m_gfxContext->Get<baregl::types::EGetParameter::SHADING_LANGUAGE_VERSION>();

	OVLOG_INFO("Graphics driver initialized:");
	OVLOG_INFO("\tVendor => " + m_vendor);
	OVLOG_INFO("\tHardware => " + m_hardware);
	OVLOG_INFO("\tVersion => " + m_version);
	OVLOG_INFO("\tShading Language => " + m_shadingLanguageVersion);
}

OvRendering::Context::Driver::~Driver()
{
	m_gfxContext.reset();
}

void OvRendering::Context::Driver::OnFrameCompleted()
{
	TracyGpuCollect;

	// Prevents state leak between frames, and especially useful when external code (like ImGui)
	// requires a "neutral" pipeline state.
	ResetPipelineState();
}

void OvRendering::Context::Driver::SetViewport(uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height)
{
	m_gfxContext->SetViewport(p_x, p_y, p_width, p_height);
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
		m_gfxContext->SetClearColor(p_color.x, p_color.y, p_color.z, p_color.w);
	}

	auto pso = CreatePipelineState();

	if (p_stencilBuffer)
	{
		pso.stencilWriteMask = ~0;
	}

	pso.scissorTest = false;

	SetPipelineState(pso);

	m_gfxContext->Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer);
}

void OvRendering::Context::Driver::Draw(
	Data::PipelineState p_pso,
	const Resources::IMesh& p_mesh,
	baregl::types::EPrimitiveMode p_primitiveMode,
	uint32_t p_instances
)
{
	ZoneScoped;

	if (p_instances > 0)
	{
		SetPipelineState(p_pso);

		p_mesh.Bind();

		if (p_mesh.GetIndexCount() > 0)
		{
			if (p_instances == 1)
			{
				m_gfxContext->DrawElements(p_primitiveMode, p_mesh.GetIndexCount());
			}
			else
			{
				m_gfxContext->DrawElementsInstanced(p_primitiveMode, p_mesh.GetIndexCount(), p_instances);
			}
		}
		else
		{
			if (p_instances == 1)
			{
				m_gfxContext->DrawArrays(p_primitiveMode, p_mesh.GetVertexCount());
			}
			else
			{
				m_gfxContext->DrawArraysInstanced(p_primitiveMode, p_mesh.GetVertexCount(), p_instances);
			}
		}

		p_mesh.Unbind();
	}
}

void OvRendering::Context::Driver::SetPipelineState(OvRendering::Data::PipelineState p_state)
{
	using namespace baregl::types;

	if (p_state._bits != m_pipelineState._bits)
	{
		auto& i = p_state;
		auto& c = m_pipelineState;

		// Rasterization
		if (i.rasterizationMode != c.rasterizationMode) m_gfxContext->SetRasterizationMode(i.rasterizationMode);
		if (i.lineWidthPow2 != c.lineWidthPow2) m_gfxContext->SetRasterizationLinesWidth(Utils::Conversions::Pow2toFloat(i.lineWidthPow2));

		if (i.colorWriting.mask != c.colorWriting.mask) m_gfxContext->SetColorWriting(i.colorWriting.r, i.colorWriting.g, i.colorWriting.b, i.colorWriting.a);
		if (i.depthWriting != c.depthWriting) m_gfxContext->SetDepthWriting(i.depthWriting);

		if (i.blending != c.blending) m_gfxContext->SetCapability(ERenderingCapability::BLEND, i.blending);
		if (i.culling != c.culling) m_gfxContext->SetCapability(ERenderingCapability::CULL_FACE, i.culling);
		if (i.dither != c.dither) m_gfxContext->SetCapability(ERenderingCapability::DITHER, i.dither);
		if (i.polygonOffsetFill != c.polygonOffsetFill) m_gfxContext->SetCapability(ERenderingCapability::POLYGON_OFFSET_FILL, i.polygonOffsetFill);
		if (i.sampleAlphaToCoverage != c.sampleAlphaToCoverage) m_gfxContext->SetCapability(ERenderingCapability::SAMPLE_ALPHA_TO_COVERAGE, i.sampleAlphaToCoverage);
		if (i.depthTest != c.depthTest) m_gfxContext->SetCapability(ERenderingCapability::DEPTH_TEST, i.depthTest);
		if (i.scissorTest != c.scissorTest) m_gfxContext->SetCapability(ERenderingCapability::SCISSOR_TEST, i.scissorTest);
		if (i.stencilTest != c.stencilTest) m_gfxContext->SetCapability(ERenderingCapability::STENCIL_TEST, i.stencilTest);
		if (i.multisample != c.multisample) m_gfxContext->SetCapability(ERenderingCapability::MULTISAMPLE, i.multisample);

		// Stencil algorithm
		if (i.stencilFuncOp != c.stencilFuncOp ||
			i.stencilFuncRef != c.stencilFuncRef ||
			i.stencilFuncMask != c.stencilFuncMask)
			m_gfxContext->SetStencilAlgorithm(i.stencilFuncOp, i.stencilFuncRef, i.stencilFuncMask);

		if (i.stencilWriteMask != c.stencilWriteMask) m_gfxContext->SetStencilMask(i.stencilWriteMask);
		if (i.stencilOpFail != c.stencilOpFail || i.depthOpFail != c.depthOpFail || i.bothOpFail != c.bothOpFail) m_gfxContext->SetStencilOperations(i.stencilOpFail, i.depthOpFail, i.bothOpFail);

		// Blending equation & function
		if (i.blendingEquation != c.blendingEquation) m_gfxContext->SetBlendingEquation(i.blendingEquation);
		if (i.blendingSrcFactor != c.blendingSrcFactor ||
			i.blendingDestFactor != c.blendingDestFactor)
			m_gfxContext->SetBlendingFunction(i.blendingSrcFactor, i.blendingDestFactor);

		// Depth
		if (i.depthFunc != c.depthFunc) m_gfxContext->SetDepthAlgorithm(i.depthFunc);

		// Culling
		if (i.cullFace != c.cullFace) m_gfxContext->SetCullFace(i.cullFace);

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
