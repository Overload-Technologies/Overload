/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/Context.h>

#include <concepts>
#include <span>
#include <vector>

#include <baregl/debug/Assert.h>
#include <baregl/debug/Log.h>
#include <baregl/data/GetResultList.h>
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

	template<typename T>
	concept SupportedGetType =
		std::same_as<T, int> || 
		std::same_as<T, int64_t> || 
		std::same_as<T, bool> || 
		std::same_as<T, float> || 
		std::same_as<T, double> ||
		std::same_as<T, std::string>;

	template<typename T>
	struct StdVectorTraits;

	template<typename T, typename Alloc>
	struct StdVectorTraits<std::vector<T, Alloc>>
	{
		using value_type = T;
	};

	template<SupportedGetType T>
	void GetValue(
		baregl::types::EGetParameter p_param,
		std::span<T> p_out
	);

	template<SupportedGetType T>
	void GetValueIndexed(
		baregl::types::EGetParameter p_param,
		std::span<T> p_out,
		uint32_t p_index
	);

	template<>
	void GetValue<int>(
		baregl::types::EGetParameter p_param,
		std::span<int> p_out
	)
	{
		glGetIntegerv(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void GetValueIndexed<int>(
		baregl::types::EGetParameter p_param,
		std::span<int> p_out,
		uint32_t p_index
	)
	{
		glGetIntegeri_v(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void GetValue<int64_t>(
		baregl::types::EGetParameter p_param,
		std::span<int64_t> p_out
	)
	{
		glGetInteger64v(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void GetValueIndexed<int64_t>(
		baregl::types::EGetParameter p_param,
		std::span<int64_t> p_out,
		uint32_t p_index
	)
	{
		glGetInteger64i_v(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void GetValue<bool>(
		baregl::types::EGetParameter p_param,
		std::span<bool> p_out
	)
	{
		std::vector<GLboolean> values(p_out.size());
		glGetBooleanv(
			baregl::utils::EnumToValue<GLenum>(p_param),
			values.data()
		);

		for (size_t i = 0; i < p_out.size(); ++i)
		{
			p_out[i] = values[i] != GL_FALSE;
		}
	}

	template<>
	void GetValueIndexed<bool>(
		baregl::types::EGetParameter p_param,
		std::span<bool> p_out,
		uint32_t p_index
	)
	{
		std::vector<GLboolean> values(p_out.size());
		glGetBooleani_v(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_index,
			values.data()
		);

		for (size_t i = 0; i < p_out.size(); ++i)
		{
			p_out[i] = values[i] != GL_FALSE;
		}
	}

	template<>
	void GetValue<float>(
		baregl::types::EGetParameter p_param,
		std::span<float> p_out
	)
	{
		glGetFloatv(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void GetValueIndexed<float>(
		baregl::types::EGetParameter p_param,
		std::span<float> p_out,
		uint32_t p_index
	)
	{
		glGetFloati_v(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void GetValue<double>(
		baregl::types::EGetParameter p_param,
		std::span<double> p_out
	)
	{
		glGetDoublev(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_out.data()
		);
	}

	template<>
	void GetValueIndexed<double>(
		baregl::types::EGetParameter p_param,
		std::span<double> p_out,
		uint32_t p_index
	)
	{
		glGetDoublei_v(
			baregl::utils::EnumToValue<GLenum>(p_param),
			p_index,
			p_out.data()
		);
	}

	template<>
	void GetValue<std::string>(
		baregl::types::EGetParameter p_param,
		std::span<std::string> p_out
	)
	{
		const GLubyte* result = glGetString(
			baregl::utils::EnumToValue<GLenum>(p_param)
		);

		p_out[0] = 
			result ?
			reinterpret_cast<const char*>(result) :
			std::string();
	}

	template<>
	void GetValueIndexed<std::string>(
		baregl::types::EGetParameter p_param,
		std::span<std::string> p_out,
		uint32_t p_index
	)
	{
		const GLubyte* result = glGetStringi(
			baregl::utils::EnumToValue<GLenum>(p_param), 
			p_index
		);

		p_out[0] = 
			result ?
			reinterpret_cast<const char*>(result) :
			std::string();
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

	template<auto PName>
		requires (data::GetResult<PName>::non_indexed)
	data::GetResultType<PName> Context::Get()
	{
		using R = data::GetResult<PName>;
		using Elem = typename R::get_type;

		if constexpr (R::dynamic_count)
		{
			using CountGet = data::GetResult<R::dynamic_count_parameter>;
			using CountElem = typename CountGet::get_type;
			using ResultElem = typename StdVectorTraits<typename R::type>::value_type;
			std::array<CountElem, CountGet::count> countRaw;
			GetValue<CountElem>(R::dynamic_count_parameter, countRaw);

			const auto elementCount = static_cast<size_t>(countRaw[0] > 0 ? countRaw[0] : 0);
			std::vector<Elem> raw(elementCount);
			if constexpr (std::same_as<Elem, std::string>)
			{
				for (size_t i = 0; i < elementCount; ++i)
				{
					std::array<Elem, 1> value;
					GetValueIndexed<Elem>(PName, value, static_cast<uint32_t>(i));
					raw[i] = value[0];
				}
			}
			else
			{
				GetValue<Elem>(PName, raw);
			}

			if constexpr (std::same_as<ResultElem, Elem>)
				return raw;
			else if constexpr (std::is_enum_v<ResultElem> && std::integral<Elem>)
			{
				std::vector<ResultElem> converted(raw.size());
				for (size_t i = 0; i < raw.size(); ++i)
					converted[i] = utils::ValueToEnum<ResultElem>(static_cast<GLenum>(raw[i]));
				return converted;
			}
			else if constexpr (std::integral<ResultElem> && std::integral<Elem>)
			{
				std::vector<ResultElem> converted(raw.size());
				for (size_t i = 0; i < raw.size(); ++i)
					converted[i] = static_cast<ResultElem>(raw[i]);
				return converted;
			}
			else
			{
				static_assert(
					std::same_as<ResultElem, Elem> ||
					(std::is_enum_v<ResultElem> && std::integral<Elem>) ||
					(std::integral<ResultElem> && std::integral<Elem>),
					"Unsupported dynamic get result type"
				);
			}

		}
		else
		{
			constexpr size_t N = R::count;
			std::array<Elem, N> raw;
			GetValue<Elem>(PName, raw);

			if constexpr (std::same_as<typename R::type, Elem>)
				return raw[0];
			else if constexpr (std::same_as<typename R::type, std::array<Elem, N>>)
				return raw;
			else if constexpr (std::integral<typename R::type> && std::integral<Elem>)
				return static_cast<typename R::type>(raw[0]);
			else if constexpr (std::is_enum_v<typename R::type>)
				return utils::ValueToEnum<typename R::type>(static_cast<GLenum>(raw[0]));

			static_assert(
				std::same_as<typename R::type, Elem> ||
				std::same_as<typename R::type, std::array<Elem, N>> ||
				(std::integral<typename R::type> && std::integral<Elem>) ||
				std::is_enum_v<typename R::type>,
				"Unsupported get result type"
			);
		}
	}

	template<auto PName>
		requires (data::GetResult<PName>::indexed)
	data::GetResultType<PName> Context::Get(uint32_t p_index)
	{
		using R = data::GetResult<PName>;
		using Elem = typename R::get_type;

		if constexpr (R::dynamic_count)
		{
			using CountGet = data::GetResult<R::dynamic_count_parameter>;
			using CountElem = typename CountGet::get_type;
			using ResultElem = typename StdVectorTraits<typename R::type>::value_type;

			std::array<CountElem, CountGet::count> countRaw;
			GetValue<CountElem>(R::dynamic_count_parameter, countRaw);

			const auto elementCount = static_cast<size_t>(countRaw[0] > 0 ? countRaw[0] : 0);
			std::vector<Elem> raw(elementCount);
			GetValueIndexed<Elem>(PName, raw, p_index);

			if constexpr (std::same_as<ResultElem, Elem>)
			{
				return raw;
			}
			else if constexpr (std::is_enum_v<ResultElem> && std::integral<Elem>)
			{
				std::vector<ResultElem> converted(raw.size());
				for (size_t i = 0; i < raw.size(); ++i)
					converted[i] = utils::ValueToEnum<ResultElem>(static_cast<GLenum>(raw[i]));
				return converted;
			}
			else if constexpr (std::integral<ResultElem> && std::integral<Elem>)
			{
				std::vector<ResultElem> converted(raw.size());
				for (size_t i = 0; i < raw.size(); ++i)
					converted[i] = static_cast<ResultElem>(raw[i]);
				return converted;
			}
			else
			{
				static_assert(
					std::same_as<ResultElem, Elem> ||
					(std::is_enum_v<ResultElem> && std::integral<Elem>) ||
					(std::integral<ResultElem> && std::integral<Elem>),
					"Unsupported dynamic indexed get result type"
				);
			}
		}
		else
		{
			constexpr size_t N = R::count;
			std::array<Elem, N> raw;
			GetValueIndexed<Elem>(PName, raw, p_index);

			if constexpr (std::same_as<typename R::type, Elem>)
				return raw[0];
			else if constexpr (std::same_as<typename R::type, std::array<Elem, N>>)
				return raw;
			else if constexpr (std::integral<typename R::type> && std::integral<Elem>)
				return static_cast<typename R::type>(raw[0]);
			else if constexpr (std::is_enum_v<typename R::type>)
				return utils::ValueToEnum<typename R::type>(static_cast<GLenum>(raw[0]));

			static_assert(
				std::same_as<typename R::type, Elem> ||
				std::same_as<typename R::type, std::array<Elem, N>> ||
				(std::integral<typename R::type> && std::integral<Elem>) ||
				std::is_enum_v<typename R::type>,
				"Unsupported indexed get result type"
			);
		}
	}

#define INSTANTIATE_GET(PARAM, GET_TYPE, COUNT_SPEC, INDEXING, ...) \
	INSTANTIATE_GET_##INDEXING(PARAM)

#define INSTANTIATE_GET_NOT_INDEXED(PARAM) \
	template data::GetResultType<baregl::types::EGetParameter::PARAM> \
	Context::Get<baregl::types::EGetParameter::PARAM>();

#define INSTANTIATE_GET_INDEXED(PARAM)

#define INSTANTIATE_GET_BOTH(PARAM) \
	template data::GetResultType<baregl::types::EGetParameter::PARAM> \
	Context::Get<baregl::types::EGetParameter::PARAM>();

#define INSTANTIATE_INDEXED_GET(PARAM, GET_TYPE, COUNT_SPEC, INDEXING, ...) \
	INSTANTIATE_INDEXED_GET_##INDEXING(PARAM)

#define INSTANTIATE_INDEXED_GET_NOT_INDEXED(PARAM)

#define INSTANTIATE_INDEXED_GET_INDEXED(PARAM) \
	template data::GetResultType<baregl::types::EGetParameter::PARAM> \
	Context::Get<baregl::types::EGetParameter::PARAM>(uint32_t);

#define INSTANTIATE_INDEXED_GET_BOTH(PARAM) \
	template data::GetResultType<baregl::types::EGetParameter::PARAM> \
	Context::Get<baregl::types::EGetParameter::PARAM>(uint32_t);

BAREGL_GET_RESULTS(INSTANTIATE_GET)
BAREGL_GET_RESULTS(INSTANTIATE_INDEXED_GET)

#undef INSTANTIATE_INDEXED_GET_BOTH
#undef INSTANTIATE_INDEXED_GET_INDEXED
#undef INSTANTIATE_INDEXED_GET_NOT_INDEXED
#undef INSTANTIATE_INDEXED_GET
#undef INSTANTIATE_GET_BOTH
#undef INSTANTIATE_GET_INDEXED
#undef INSTANTIATE_GET_NOT_INDEXED
#undef INSTANTIATE_GET

}
