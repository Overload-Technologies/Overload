/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/Context.h>

#include <baregl/debug/Log.h>
#include <baregl/debug/Assert.h>
#include <baregl/detail/Types.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/math/Conversions.h>
#include <baregl/utils/BitmaskOperators.h>
#include <baregl/utils/EnumMapper.h>

namespace
{
	uint32_t g_contextCount = 0u;

	void GLDebugMessageCallback(uint32_t p_source, uint32_t p_type, uint32_t p_id, uint32_t p_severity, int32_t p_length, const char* p_message, const void* p_userParam)
	{
		// Ignore non-significant error/warning codes
		if (p_id == 131169 || p_id == 131185 || p_id == 131218 || p_id == 131204)
		{
			return;
		}

		std::string output = "OpenGL Debug Message:\n";
		output += "Debug message (" + std::to_string(p_id) + "): " + p_message + "\n";

		switch (p_source)
		{
		case GL_DEBUG_SOURCE_API:				output += "Source: API";				break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		output += "Source: Window System";		break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:	output += "Source: Shader Compiler";	break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:		output += "Source: Third Party";		break;
		case GL_DEBUG_SOURCE_APPLICATION:		output += "Source: Application";		break;
		case GL_DEBUG_SOURCE_OTHER:				output += "Source: Other";				break;
		}

		output += "\n";

		switch (p_type)
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

		switch (p_severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:			output += "Severity: High";				BAREGL_LOG_ERROR(output);	break;
		case GL_DEBUG_SEVERITY_MEDIUM:			output += "Severity: Medium";			BAREGL_LOG_WARNING(output);	break;
		case GL_DEBUG_SEVERITY_LOW:				output += "Severity: Low";				BAREGL_LOG_INFO(output);	break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:	output += "Severity: Notification";		BAREGL_LOG_INFO(output);	break;
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
}

namespace baregl
{
	Context::Context(const baregl::data::ContextDesc& p_desc)
	{
		BAREGL_ASSERT(g_contextCount == 0, "A context already exists. BareGL currently only supports a single context.");

		++g_contextCount;

		const int error = gladLoadGL();

		if (error == 0)
		{
			BAREGL_LOG_ERROR("GLAD failed to initialize");
			return;
		}

		BAREGL_LOG_INFO("OpenGL context initialized.");

		if (p_desc.debug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		}

		// Seamless cubemap (always on)
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);
	}

	Context::~Context()
	{
		BAREGL_ASSERT(g_contextCount > 0, "No context to destroy, did you modify g_contextCount manually?");

		--g_contextCount;
	}

	void Context::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer)
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

	void Context::DrawElements(types::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount)
	{
		glDrawElements(utils::EnumToValue<GLenum>(p_primitiveMode), p_indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void Context::DrawElementsInstanced(types::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount, uint32_t p_instances)
	{
		glDrawElementsInstanced(utils::EnumToValue<GLenum>(p_primitiveMode), p_indexCount, GL_UNSIGNED_INT, nullptr, p_instances);
	}

	void Context::DrawArrays(types::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount)
	{
		glDrawArrays(utils::EnumToValue<GLenum>(p_primitiveMode), 0, p_vertexCount);
	}

	void Context::DrawArraysInstanced(types::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount, uint32_t p_instances)
	{
		glDrawArraysInstanced(utils::EnumToValue<GLenum>(p_primitiveMode), 0, p_vertexCount, p_instances);
	}

	void Context::DispatchCompute(uint32_t p_x, uint32_t p_y, uint32_t p_z) const
	{
		BAREGL_ASSERT(
			p_x > 0 && p_y > 0 && p_z > 0,
			"Dispatch work group count cannot be zero"
		);

		glDispatchCompute(p_x, p_y, p_z);
	}

	void Context::MemoryBarrier(types::EMemoryBarrierFlags p_barriers) const
	{
		glMemoryBarrier(
			utils::EnumToValue<GLbitfield>(p_barriers)
		);
	}

	void Context::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha)
	{
		glClearColor(p_red, p_green, p_blue, p_alpha);
	}

	void Context::SetRasterizationLinesWidth(float p_width)
	{
		glLineWidth(p_width);
	}

	void Context::SetRasterizationMode(types::ERasterizationMode p_rasterizationMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, utils::EnumToValue<GLenum>(p_rasterizationMode));
	}

	void Context::SetCapability(types::ERenderingCapability p_capability, bool p_value)
	{
		(p_value ? glEnable : glDisable)(utils::EnumToValue<GLenum>(p_capability));
	}

	bool Context::GetCapability(types::ERenderingCapability p_capability)
	{
		return glIsEnabled(utils::EnumToValue<GLenum>(p_capability));
	}

	void Context::SetStencilAlgorithm(types::EComparaisonAlgorithm p_algorithm, int32_t p_reference, uint32_t p_mask)
	{
		glStencilFunc(utils::EnumToValue<GLenum>(p_algorithm), p_reference, p_mask);
	}

	void Context::SetDepthAlgorithm(types::EComparaisonAlgorithm p_algorithm)
	{
		glDepthFunc(utils::EnumToValue<GLenum>(p_algorithm));
	}

	void Context::SetStencilMask(uint32_t p_mask)
	{
		glStencilMask(p_mask);
	}

	void Context::SetStencilOperations(types::EOperation p_stencilFail, types::EOperation p_depthFail, types::EOperation p_bothPass)
	{
		glStencilOp(
			utils::EnumToValue<GLenum>(p_stencilFail),
			utils::EnumToValue<GLenum>(p_depthFail),
			utils::EnumToValue<GLenum>(p_bothPass)
		);
	}

	void Context::SetBlendingFunction(types::EBlendingFactor p_sourceFactor, types::EBlendingFactor p_destinationFactor)
	{
		glBlendFunc(
			utils::EnumToValue<GLenum>(p_sourceFactor),
			utils::EnumToValue<GLenum>(p_destinationFactor)
		);
	}

	void Context::SetBlendingEquation(types::EBlendingEquation p_equation)
	{
		glBlendEquation(utils::EnumToValue<GLenum>(p_equation));
	}

	void Context::SetCullFace(types::ECullFace p_cullFace)
	{
		glCullFace(utils::EnumToValue<GLenum>(p_cullFace));
	}

	void Context::SetDepthWriting(bool p_enable)
	{
		glDepthMask(p_enable);
	}

	void Context::SetColorWriting(bool p_enableRed, bool p_enableGreen, bool p_enableBlue, bool p_enableAlpha)
	{
		glColorMask(p_enableRed, p_enableGreen, p_enableBlue, p_enableAlpha);
	}

	void Context::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	std::string Context::GetVendor()
	{
		return GetString(GL_VENDOR);
	}

	std::string Context::GetHardware()
	{
		return GetString(GL_RENDERER);
	}

	std::string Context::GetVersion()
	{
		return GetString(GL_VERSION);
	}

	std::string Context::GetShadingLanguageVersion()
	{
		return GetString(GL_SHADING_LANGUAGE_VERSION);
	}
	
	template<>
	void Context::GetValue<int>(
		types::EGetParameter p_param,
		std::span<int> p_out
	)
	{
		glGetIntegerv(
			utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void Context::GetValueIndexed<int>(
		types::EGetParameter p_param,
		std::span<int> p_out,
		uint32_t p_index
	)
	{
		glGetIntegeri_v(
			utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void Context::GetValue<int64_t>(
		types::EGetParameter p_param,
		std::span<int64_t> p_out
	)
	{
		glGetInteger64v(
			utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void Context::GetValueIndexed<int64_t>(
		types::EGetParameter p_param,
		std::span<int64_t> p_out,
		uint32_t p_index
	)
	{
		glGetInteger64i_v(
			utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void Context::GetValue<bool>(
		types::EGetParameter p_param,
		std::span<bool> p_out
	)
	{
		glGetBooleanv(
			utils::EnumToValue<GLenum>(p_param),
			reinterpret_cast<GLboolean*>(p_out.data())
		);
	}

	template<>
	void Context::GetValueIndexed<bool>(
		types::EGetParameter p_param,
		std::span<bool> p_out,
		uint32_t p_index
	)
	{
		glGetBooleani_v(
			utils::EnumToValue<GLenum>(p_param),
			p_index,
			reinterpret_cast<GLboolean*>(p_out.data())
		);
	}

	template<>
	void Context::GetValue<float>(
		types::EGetParameter p_param,
		std::span<float> p_out
	)
	{
		glGetFloatv(
			utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void Context::GetValueIndexed<float>(
		types::EGetParameter p_param,
		std::span<float> p_out,
		uint32_t p_index
	)
	{
		glGetFloati_v(
			utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void Context::GetValue<double>(
		types::EGetParameter p_param,
		std::span<double> p_out
	)
	{
		glGetDoublev(
			utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void Context::GetValueIndexed<double>(
		types::EGetParameter p_param,
		std::span<double> p_out,
		uint32_t p_index
	)
	{
		glGetDoublei_v(
			utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}
}
