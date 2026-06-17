/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <span>

#include <glad.h>

// Needs to be included after OpenGL headers
#include <tracy/TracyOpenGL.hpp>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/OpenGL/GLBackend.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>
#include <OvRendering/Utils/Conversions.h>

namespace
{
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

	/**
	* Constrains the OpenGL state query helpers to the value types OpenGL can return.
	*/
	template<typename T>
	concept SupportedGetType =
		std::same_as<T, bool> ||
		std::same_as<T, int> ||
		std::same_as<T, int64_t> ||
		std::same_as<T, float> ||
		std::same_as<T, double>;

	/**
	* Upper bound for the scratch buffer used when the requested type does not match
	* the OpenGL native type (bool, and int64_t on some platforms). No such query
	* returns more than four values (GL_COLOR_WRITEMASK returns four).
	*/
	constexpr size_t kMaximumConvertedValueCount = 4;

	/**
	* Reads one or more values for an OpenGL parameter into a caller-provided buffer.
	* @param p_parameter The OpenGL parameter to query.
	* @param p_output The buffer the values are written into.
	*/
	template<SupportedGetType T>
	void GetValue(uint32_t p_parameter, std::span<T> p_output)
	{
		if constexpr (std::same_as<T, bool>)
		{
			std::array<GLboolean, kMaximumConvertedValueCount> nativeResult{};
			glGetBooleanv(p_parameter, nativeResult.data());

			for (size_t i = 0; i < p_output.size() && i < nativeResult.size(); ++i)
			{
				p_output[i] = static_cast<bool>(nativeResult[i]);
			}
		}
		else if constexpr (std::same_as<T, int64_t>)
		{
			std::array<GLint64, kMaximumConvertedValueCount> nativeResult{};
			glGetInteger64v(p_parameter, nativeResult.data());

			for (size_t i = 0; i < p_output.size() && i < nativeResult.size(); ++i)
			{
				p_output[i] = static_cast<int64_t>(nativeResult[i]);
			}
		}
		else if constexpr (std::same_as<T, int>)
		{
			glGetIntegerv(p_parameter, p_output.data());
		}
		else if constexpr (std::same_as<T, float>)
		{
			glGetFloatv(p_parameter, p_output.data());
		}
		else if constexpr (std::same_as<T, double>)
		{
			glGetDoublev(p_parameter, p_output.data());
		}
	}

	/**
	* Reads a single value for an OpenGL parameter.
	* Use BufferSize to size the read buffer for parameters that return more than one value.
	* @param p_parameter The OpenGL parameter to query.
	*/
	template<SupportedGetType T, size_t BufferSize = 1>
	T GetValue(uint32_t p_parameter)
	{
		std::array<T, BufferSize> result{};
		GetValue<T>(p_parameter, std::span<T>(result));
		return result[0];
	}

	/**
	* Reads one or more values for an indexed OpenGL parameter into a caller-provided buffer.
	* @param p_parameter The OpenGL parameter to query.
	* @param p_output The buffer the values are written into.
	* @param p_index The index of the element to query.
	*/
	template<SupportedGetType T>
	void GetValueIndexed(uint32_t p_parameter, std::span<T> p_output, uint32_t p_index)
	{
		if constexpr (std::same_as<T, bool>)
		{
			std::array<GLboolean, kMaximumConvertedValueCount> nativeResult{};
			glGetBooleani_v(p_parameter, p_index, nativeResult.data());

			for (size_t i = 0; i < p_output.size() && i < nativeResult.size(); ++i)
			{
				p_output[i] = static_cast<bool>(nativeResult[i]);
			}
		}
		else if constexpr (std::same_as<T, int64_t>)
		{
			std::array<GLint64, kMaximumConvertedValueCount> nativeResult{};
			glGetInteger64i_v(p_parameter, p_index, nativeResult.data());

			for (size_t i = 0; i < p_output.size() && i < nativeResult.size(); ++i)
			{
				p_output[i] = static_cast<int64_t>(nativeResult[i]);
			}
		}
		else if constexpr (std::same_as<T, int>)
		{
			glGetIntegeri_v(p_parameter, p_index, p_output.data());
		}
		else if constexpr (std::same_as<T, float>)
		{
			glGetFloati_v(p_parameter, p_index, p_output.data());
		}
		else if constexpr (std::same_as<T, double>)
		{
			glGetDoublei_v(p_parameter, p_index, p_output.data());
		}
	}

	/**
	* Reads a single value for an indexed OpenGL parameter.
	* Use BufferSize to size the read buffer for parameters that return more than one value.
	* @param p_parameter The OpenGL parameter to query.
	* @param p_index The index of the element to query.
	*/
	template<SupportedGetType T, size_t BufferSize = 1>
	T GetValueIndexed(uint32_t p_parameter, uint32_t p_index)
	{
		std::array<T, BufferSize> result{};
		GetValueIndexed<T>(p_parameter, std::span<T>(result), p_index);
		return result[0];
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
		using namespace OvRendering::HAL;

		OvRendering::Data::PipelineState pso;

		// Rasterization
		pso.rasterizationMode = static_cast<ERasterizationMode>(GetValue<int, 2>(GL_POLYGON_MODE));
		pso.lineWidthPow2 = OvRendering::Utils::Conversions::FloatToPow2(GetValue<float>(GL_LINE_WIDTH));

		// Color write mask
		std::array<bool, 4> colorWriteMask{};
		GetValue<bool>(GL_COLOR_WRITEMASK, colorWriteMask);
		pso.colorWriting.r = colorWriteMask[0];
		pso.colorWriting.g = colorWriteMask[1];
		pso.colorWriting.b = colorWriteMask[2];
		pso.colorWriting.a = colorWriteMask[3];

		// Capability
		pso.depthWriting = GetValue<bool>(GL_DEPTH_WRITEMASK);
		pso.blending = GetValue<bool>(GL_BLEND);
		pso.culling = GetValue<bool>(GL_CULL_FACE);
		pso.dither = GetValue<bool>(GL_DITHER);
		pso.polygonOffsetFill = GetValue<bool>(GL_POLYGON_OFFSET_FILL);
		pso.sampleAlphaToCoverage = GetValue<bool>(GL_SAMPLE_ALPHA_TO_COVERAGE);
		pso.depthTest = GetValue<bool>(GL_DEPTH_TEST);
		pso.scissorTest = GetValue<bool>(GL_SCISSOR_TEST);
		pso.stencilTest = GetValue<bool>(GL_STENCIL_TEST);
		pso.multisample = GetValue<bool>(GL_MULTISAMPLE);

		// Stencil
		pso.stencilFuncOp = ValueToEnum<EComparaisonAlgorithm>(static_cast<GLenum>(GetValue<int>(GL_STENCIL_FUNC)));
		pso.stencilFuncRef = GetValue<int>(GL_STENCIL_REF);
		pso.stencilFuncMask = static_cast<uint32_t>(GetValue<int>(GL_STENCIL_VALUE_MASK));

		pso.stencilWriteMask = static_cast<uint32_t>(GetValue<int>(GL_STENCIL_WRITEMASK));

		pso.stencilOpFail = ValueToEnum<EOperation>(static_cast<GLenum>(GetValue<int>(GL_STENCIL_FAIL)));
		pso.depthOpFail = ValueToEnum<EOperation>(static_cast<GLenum>(GetValue<int>(GL_STENCIL_PASS_DEPTH_FAIL)));
		pso.bothOpFail = ValueToEnum<EOperation>(static_cast<GLenum>(GetValue<int>(GL_STENCIL_PASS_DEPTH_PASS)));

		// Depth
		pso.depthFunc = ValueToEnum<EComparaisonAlgorithm>(static_cast<GLenum>(GetValue<int>(GL_DEPTH_FUNC)));

		// Culling
		pso.cullFace = ValueToEnum<ECullFace>(static_cast<GLenum>(GetValue<int>(GL_CULL_FACE_MODE)));

		// Blending
		pso.blendingSrcFactor = ValueToEnum<EBlendingFactor>(static_cast<GLenum>(GetValue<int>(GL_BLEND_SRC)));
		pso.blendingDestFactor = ValueToEnum<EBlendingFactor>(static_cast<GLenum>(GetValue<int>(GL_BLEND_DST)));
		pso.blendingEquation = ValueToEnum<EBlendingEquation>(static_cast<GLenum>(GetValue<int>(GL_BLEND_EQUATION)));

		return pso;
	}
}

namespace OvRendering::HAL
{
	template<>
	std::optional<Data::PipelineState> GLBackend::Init(bool debug)
	{
		const int error = gladLoadGL();

		if (error == 0)
		{
			OVLOG_ERROR("GLAD failed to initialize");
			return std::nullopt;
		}

		TracyGpuContext;

		if (debug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		}

		// Seamless cubemap (always on)
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		return RetrieveOpenGLPipelineState();
	}

	template<>
	void GLBackend::OnFrameCompleted()
	{
		TracyGpuCollect;
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
	void GLBackend::SetBlendingFunction(Settings::EBlendingFactor p_sourceFactor, Settings::EBlendingFactor p_destinationFactor)
	{
		glBlendFunc(
			EnumToValue<GLenum>(p_sourceFactor),
			EnumToValue<GLenum>(p_destinationFactor)
		);
	}

	template<>
	void GLBackend::SetBlendingEquation(Settings::EBlendingEquation p_equation)
	{
		glBlendEquation(EnumToValue<GLenum>(p_equation));
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
