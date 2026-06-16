/**
 * @project: baregl
 * @author: Adrien Givry
 * @licence: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include <baregl/data/GetResultList.h>
#include <baregl/types/EBlendingEquation.h>
#include <baregl/types/EBlendingFactor.h>
#include <baregl/types/EComparaisonAlgorithm.h>
#include <baregl/types/EContextFlags.h>
#include <baregl/types/ECullFace.h>
#include <baregl/types/EDrawBuffer.h>
#include <baregl/types/EGetParameter.h>
#include <baregl/types/EHint.h>
#include <baregl/types/EImageAccessSpecifier.h>
#include <baregl/types/EInternalFormat.h>
#include <baregl/types/ELogicOperation.h>
#include <baregl/types/ETextureUnit.h>
#include <baregl/types/EPixelDataFormat.h>
#include <baregl/types/EPixelDataType.h>
#include <baregl/types/EOperation.h>
#include <baregl/types/EProvokingVertexConvention.h>
#include <baregl/types/ERasterizationMode.h>

namespace baregl::data
{
	template<auto PName> struct GetResult;

	enum class EGetIndexing
	{
		INDEXED,
		NOT_INDEXED,
		BOTH
	};

	template<std::size_t N>
	struct FixedCount
	{
		static constexpr bool dynamic = false;
		static constexpr std::size_t value = N;
	};

	template<types::EGetParameter P>
	struct DynamicCount
	{
		static constexpr bool dynamic = true;
		static constexpr types::EGetParameter parameter = P;
	};

	template<typename CountSpec>
	struct CountSpecTraits;

	template<std::size_t N>
	struct CountSpecTraits<FixedCount<N>>
	{
		static constexpr bool dynamic = false;
		static constexpr std::size_t count = N;
		static constexpr types::EGetParameter dynamic_count_parameter = types::EGetParameter::PATCH_VERTICES;
	};

	template<types::EGetParameter P>
	struct CountSpecTraits<DynamicCount<P>>
	{
		static constexpr bool dynamic = true;
		static constexpr std::size_t count = 0;
		static constexpr types::EGetParameter dynamic_count_parameter = P;
	};

	#define DECLARE_GET_RESULT(PARAM, GET_TYPE, COUNT_SPEC, INDEXING, ...) \
		template <> struct GetResult<types::EGetParameter::PARAM> { \
			using count_spec = COUNT_SPEC; \
			using count_traits = CountSpecTraits<count_spec>; \
			using value_type = __VA_ARGS__; \
			using type = std::conditional_t<count_traits::dynamic, std::vector<value_type>, value_type>; \
			using get_type = GET_TYPE; \
			static constexpr std::size_t count = count_traits::count; \
			static constexpr EGetIndexing indexing = EGetIndexing::INDEXING; \
			static constexpr bool non_indexed = indexing != EGetIndexing::INDEXED; \
			static constexpr bool indexed = indexing != EGetIndexing::NOT_INDEXED; \
			static constexpr bool dynamic_count = count_traits::dynamic; \
			static constexpr types::EGetParameter dynamic_count_parameter = count_traits::dynamic_count_parameter; \
		};

	BAREGL_GET_RESULTS(DECLARE_GET_RESULT)

	#undef DECLARE_GET_RESULT

	template<auto P>
	using GetResultType = typename GetResult<P>::type;
}
