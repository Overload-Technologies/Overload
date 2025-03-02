/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

/**
* Although this is intended as a Null implementation, certain components of the engine rely on OpenGL,
* such as resource creation, binding, and the user interface. Consequently, GLEW must be initialized to
* support these functionalities. This implementation exclusively initializes GLEW without making any
* additional calls.
*/
#include <GL/glew.h>

#include <OvRendering/HAL/None/NoneBackend.h>

namespace OvRendering::HAL
{
	template<>
	std::optional<OvRendering::Data::PipelineState> NoneBackend::Init(bool debug)
	{
		glewInit(); // <-- initialize GLEW; see comment above the GLEW include directive for more details.
		return OvRendering::Data::PipelineState{};
	}

	template<>
	void NoneBackend::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer)
	{}

	template<>
	void NoneBackend::DrawElements(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount)
	{}

	template<>
	void NoneBackend::DrawElementsInstanced(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount, uint32_t p_instances)
	{}

	template<>
	void NoneBackend::DrawArrays(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount)
	{}

	template<>
	void NoneBackend::DrawArraysInstanced(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount, uint32_t p_instances)
	{}

	template<>
	void NoneBackend::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha)
	{}

	template<>
	void NoneBackend::SetRasterizationLinesWidth(float p_width)
	{}

	template<>
	void NoneBackend::SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode)
	{}

	template<>
	void NoneBackend::SetCapability(Settings::ERenderingCapability p_capability, bool p_value)
	{}

	template<>
	bool NoneBackend::GetCapability(Settings::ERenderingCapability p_capability)
	{
		return false;
	}

	template<>
	void NoneBackend::SetStencilAlgorithm(Settings::EComparaisonAlgorithm p_algorithm, int32_t p_reference, uint32_t p_mask)
	{}

	template<>
	void NoneBackend::SetDepthAlgorithm(Settings::EComparaisonAlgorithm p_algorithm)
	{}

	template<>
	void NoneBackend::SetStencilMask(uint32_t p_mask)
	{}

	template<>
	void NoneBackend::SetStencilOperations(Settings::EOperation p_stencilFail, Settings::EOperation p_depthFail, Settings::EOperation p_bothPass)
	{}

	template<>
	void NoneBackend::SetCullFace(Settings::ECullFace p_cullFace)
	{}

	template<>
	void NoneBackend::SetDepthWriting(bool p_enable)
	{}

	template<>
	void NoneBackend::SetColorWriting(bool p_enableRed, bool p_enableGreen, bool p_enableBlue, bool p_enableAlpha)
	{}

	template<>
	void NoneBackend::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{}

	template<>
	std::string NoneBackend::GetVendor()
	{
		return "None";
	}

	template<>
	std::string NoneBackend::GetHardware()
	{
		return "None";
	}

	template<>
	std::string NoneBackend::GetVersion()
	{
		return "None";
	}

	template<>
	std::string NoneBackend::GetShadingLanguageVersion()
	{
		return "None";
	}
}
