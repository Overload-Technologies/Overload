/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <GL/glew.h>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/OpenGL/GLBackend.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>
#include <OvRendering/Utils/Conversions.h>

void GLDebugMessageCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char* message, const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::string output;

	output += "OpenGL Debug Message:\n";
	output += "Debug message (" + std::to_string(id) + "): " + message + "\n";

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:				output += "Source: API";				break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		output += "Source: Window System";		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	output += "Source: Shader Compiler";	break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:		output += "Source: Third Party";		break;
	case GL_DEBUG_SOURCE_APPLICATION:		output += "Source: Application";		break;
	case GL_DEBUG_SOURCE_OTHER:				output += "Source: Other";				break;
	}

	output += "\n";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               output += "Type: Error";				break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: output += "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  output += "Type: Undefined Behaviour";	break;
	case GL_DEBUG_TYPE_PORTABILITY:         output += "Type: Portability";			break;
	case GL_DEBUG_TYPE_PERFORMANCE:         output += "Type: Performance";			break;
	case GL_DEBUG_TYPE_MARKER:              output += "Type: Marker";				break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          output += "Type: Push Group";			break;
	case GL_DEBUG_TYPE_POP_GROUP:           output += "Type: Pop Group";			break;
	case GL_DEBUG_TYPE_OTHER:               output += "Type: Other";				break;
	}

	output += "\n";

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:			output += "Severity: High";				break;
	case GL_DEBUG_SEVERITY_MEDIUM:			output += "Severity: Medium";			break;
	case GL_DEBUG_SEVERITY_LOW:				output += "Severity: Low";				break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	output += "Severity: Notification";		break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:			OVLOG_ERROR(output);	break;
	case GL_DEBUG_SEVERITY_MEDIUM:			OVLOG_WARNING(output);	break;
	case GL_DEBUG_SEVERITY_LOW:				OVLOG_INFO(output);		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	OVLOG_INFO(output);		break;
	}
}

bool GetBool(uint32_t p_parameter)
{
	GLboolean result;
	glGetBooleanv(p_parameter, &result);
	return static_cast<bool>(result);
}

bool GetBool(uint32_t p_parameter, uint32_t p_index)
{
	GLboolean result;
	glGetBooleani_v(p_parameter, p_index, &result);
	return static_cast<bool>(result);
}

int GetInt(uint32_t p_parameter)
{
	GLint result;
	glGetIntegerv(p_parameter, &result);
	return static_cast<int>(result);
}

int GetInt(uint32_t p_parameter, uint32_t p_index)
{
	GLint result;
	glGetIntegeri_v(p_parameter, p_index, &result);
	return static_cast<int>(result);
}

float GetFloat(uint32_t p_parameter)
{
	GLfloat result;
	glGetFloatv(p_parameter, &result);
	return static_cast<float>(result);
}

float GetFloat(uint32_t p_parameter, uint32_t p_index)
{
	GLfloat result;
	glGetFloati_v(p_parameter, p_index, &result);
	return static_cast<float>(result);
}

double GetDouble(uint32_t p_parameter)
{
	GLdouble result;
	glGetDoublev(p_parameter, &result);
	return static_cast<double>(result);
}

double GetDouble(uint32_t p_parameter, uint32_t p_index)
{
	GLdouble result;
	glGetDoublei_v(p_parameter, p_index, &result);
	return static_cast<double>(result);
}

int64_t GetInt64(uint32_t p_parameter)
{
	GLint64 result;
	glGetInteger64v(p_parameter, &result);
	return static_cast<int64_t>(result);
}

int64_t GetInt64(uint32_t p_parameter, uint32_t p_index)
{
	GLint64 result;
	glGetInteger64i_v(p_parameter, p_index, &result);
	return static_cast<int64_t>(result);
}

std::string GetString(uint32_t p_parameter)
{
	const GLubyte* result = glGetString(p_parameter);
	return result ? reinterpret_cast<const char*>(result) : std::string();
}

std::string GetString(uint32_t p_parameter, uint32_t p_index)
{
	const GLubyte* result = glGetStringi(p_parameter, p_index);
	return result ? reinterpret_cast<const char*>(result) : std::string();
}

/**
* Very expensive! Call it once, and make sure you always keep track of state changes
*/
OvRendering::Data::PipelineState RetrieveOpenGLPipelineState()
{
	using namespace OvRendering::Settings;

	OvRendering::Data::PipelineState pso;

	// Rasterization
	pso.rasterizationMode = static_cast<ERasterizationMode>(GetInt(GL_POLYGON_MODE));
	pso.lineWidthPow2 = OvRendering::Utils::Conversions::FloatToPow2(GetFloat(GL_LINE_WIDTH));

	// Color write mask
	GLboolean colorWriteMask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, colorWriteMask);
	pso.colorWriting.r = colorWriteMask[0];
	pso.colorWriting.g = colorWriteMask[1];
	pso.colorWriting.b = colorWriteMask[2];
	pso.colorWriting.a = colorWriteMask[3];

	// Capability
	pso.depthWriting = GetBool(GL_DEPTH_WRITEMASK);
	pso.blending = GetBool(GL_BLEND);
	pso.culling = GetBool(GL_CULL_FACE);
	pso.dither = GetBool(GL_DITHER);
	pso.polygonOffsetFill = GetBool(GL_POLYGON_OFFSET_FILL);
	pso.sampleAlphaToCoverage = GetBool(GL_SAMPLE_ALPHA_TO_COVERAGE);
	pso.depthTest = GetBool(GL_DEPTH_TEST);
	pso.scissorTest = GetBool(GL_SCISSOR_TEST);
	pso.stencilTest = GetBool(GL_STENCIL_TEST);
	pso.multisample = GetBool(GL_MULTISAMPLE);

	// Stencil
	pso.stencilFuncOp = static_cast<EComparaisonAlgorithm>(GetInt(GL_STENCIL_FUNC));
	pso.stencilFuncRef = GetInt(GL_STENCIL_REF);
	pso.stencilFuncMask = static_cast<uint32_t>(GetInt(GL_STENCIL_VALUE_MASK));

	pso.stencilWriteMask = static_cast<uint32_t>(GetInt(GL_STENCIL_WRITEMASK));

	pso.stencilOpFail = static_cast<EOperation>(GetInt(GL_STENCIL_FAIL));
	pso.depthOpFail = static_cast<EOperation>(GetInt(GL_STENCIL_PASS_DEPTH_FAIL));
	pso.bothOpFail = static_cast<EOperation>(GetInt(GL_STENCIL_PASS_DEPTH_PASS));

	// Depth
	pso.depthFunc = static_cast<EComparaisonAlgorithm>(GetInt(GL_DEPTH_FUNC));

	// Culling
	pso.cullFace = static_cast<ECullFace>(GetInt(GL_CULL_FACE_MODE));

	return pso;
}

namespace OvRendering::HAL
{
	template<>
	std::optional<Data::PipelineState> GLBackend::Init(bool debug)
	{
		const GLenum error = glewInit();

		if (error != GLEW_OK)
		{
			std::string message = "Error Init GLEW: ";
			std::string glewError = reinterpret_cast<const char*>(glewGetErrorString(error));
			OVLOG_ERROR(message + glewError);
			return std::nullopt;
		}

		if (debug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		return RetrieveOpenGLPipelineState();
	}

	template<>
	void GLBackend::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer)
	{
		GLbitfield clearMask = 0;

		if (p_colorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
		if (p_depthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;
		if (p_stencilBuffer) clearMask |= GL_STENCIL_BUFFER_BIT;

		if (clearMask != 0)
		{
			glClear(clearMask);
		}
	}

	template<>
	void GLBackend::DrawElements(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount)
	{
		glDrawElements(EnumToValue<GLenum>(p_primitiveMode), p_indexCount, GL_UNSIGNED_INT, nullptr);
	}

	template<>
	void GLBackend::DrawElementsInstanced(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount, uint32_t p_instances)
	{
		glDrawElementsInstanced(EnumToValue<GLenum>(p_primitiveMode), p_indexCount, GL_UNSIGNED_INT, nullptr, p_instances);
	}

	template<>
	void GLBackend::DrawArrays(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount)
	{
		glDrawArrays(EnumToValue<GLenum>(p_primitiveMode), 0, p_vertexCount);
	}

	template<>
	void GLBackend::DrawArraysInstanced(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount, uint32_t p_instances)
	{
		glDrawArraysInstanced(EnumToValue<GLenum>(p_primitiveMode), 0, p_vertexCount, p_instances);
	}

	template<>
	void GLBackend::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha)
	{
		glClearColor(p_red, p_green, p_blue, p_alpha);
	}

	template<>
	void GLBackend::SetRasterizationLinesWidth(float p_width)
	{
		glLineWidth(p_width);
	}

	template<>
	void GLBackend::SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, EnumToValue<GLenum>(p_rasterizationMode));
	}

	template<>
	void GLBackend::SetCapability(Settings::ERenderingCapability p_capability, bool p_value)
	{
		(p_value ? glEnable : glDisable)(EnumToValue<GLenum>(p_capability));
	}

	template<>
	bool GLBackend::GetCapability(Settings::ERenderingCapability p_capability)
	{
		return glIsEnabled(EnumToValue<GLenum>(p_capability));
	}

	template<>
	void GLBackend::SetStencilAlgorithm(Settings::EComparaisonAlgorithm p_algorithm, int32_t p_reference, uint32_t p_mask)
	{
		glStencilFunc(EnumToValue<GLenum>(p_algorithm), p_reference, p_mask);
	}

	template<>
	void GLBackend::SetDepthAlgorithm(Settings::EComparaisonAlgorithm p_algorithm)
	{
		glDepthFunc(EnumToValue<GLenum>(p_algorithm));
	}

	template<>
	void GLBackend::SetStencilMask(uint32_t p_mask)
	{
		glStencilMask(p_mask);
	}

	template<>
	void GLBackend::SetStencilOperations(Settings::EOperation p_stencilFail, Settings::EOperation p_depthFail, Settings::EOperation p_bothPass)
	{
		glStencilOp(EnumToValue<GLenum>(p_stencilFail), EnumToValue<GLenum>(p_depthFail), EnumToValue<GLenum>(p_bothPass));
	}

	template<>
	void GLBackend::SetCullFace(Settings::ECullFace p_cullFace)
	{
		glCullFace(EnumToValue<GLenum>(p_cullFace));
	}

	template<>
	void GLBackend::SetDepthWriting(bool p_enable)
	{
		glDepthMask(p_enable);
	}

	template<>
	void GLBackend::SetColorWriting(bool p_enableRed, bool p_enableGreen, bool p_enableBlue, bool p_enableAlpha)
	{
		glColorMask(p_enableRed, p_enableGreen, p_enableBlue, p_enableAlpha);
	}

	template<>
	void GLBackend::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	template<>
	std::string GLBackend::GetVendor()
	{
		return GetString(GL_VENDOR);
	}

	template<>
	std::string GLBackend::GetHardware()
	{
		return GetString(GL_RENDERER);
	}

	template<>
	std::string GLBackend::GetVersion()
	{
		return GetString(GL_VERSION);
	}

	template<>
	std::string GLBackend::GetShadingLanguageVersion()
	{
		return GetString(GL_SHADING_LANGUAGE_VERSION);
	}
}
