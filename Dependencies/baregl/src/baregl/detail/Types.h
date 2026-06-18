/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/detail/glad/glad.h>
#include <baregl/types/EAccessSpecifier.h>
#include <baregl/types/EBlendingEquation.h>
#include <baregl/types/EBlendingFactor.h>
#include <baregl/types/EBufferType.h>
#include <baregl/types/EComparaisonAlgorithm.h>
#include <baregl/types/EContextFlags.h>
#include <baregl/types/ECullFace.h>
#include <baregl/types/EDataType.h>
#include <baregl/types/EDrawBuffer.h>
#include <baregl/types/EFormat.h>
#include <baregl/types/EFramebufferAttachment.h>
#include <baregl/types/EGetParameter.h>
#include <baregl/types/EHint.h>
#include <baregl/types/EImageAccessSpecifier.h>
#include <baregl/types/EInternalFormat.h>
#include <baregl/types/ELogicOperation.h>
#include <baregl/types/EMemoryBarrierFlags.h>
#include <baregl/types/EOperation.h>
#include <baregl/types/EPixelDataFormat.h>
#include <baregl/types/EPixelDataType.h>
#include <baregl/types/EPrimitiveMode.h>
#include <baregl/types/EProvokingVertexConvention.h>
#include <baregl/types/ERasterizationMode.h>
#include <baregl/types/ERenderingCapability.h>
#include <baregl/types/EShaderType.h>
#include <baregl/types/ETextureFilteringMode.h>
#include <baregl/types/ETextureType.h>
#include <baregl/types/ETextureUnit.h>
#include <baregl/types/ETextureWrapMode.h>
#include <baregl/types/EUniformType.h>
#include <baregl/utils/EnumMapper.h>

template <>
struct baregl::utils::MappingFor<baregl::types::EComparaisonAlgorithm, GLenum>
{
	using EnumType = baregl::types::EComparaisonAlgorithm;
	using type = std::tuple<
		EnumValuePair<EnumType::NEVER, GL_NEVER>,
		EnumValuePair<EnumType::LESS, GL_LESS>,
		EnumValuePair<EnumType::EQUAL, GL_EQUAL>,
		EnumValuePair<EnumType::LESS_EQUAL, GL_LEQUAL>,
		EnumValuePair<EnumType::GREATER, GL_GREATER>,
		EnumValuePair<EnumType::NOTEQUAL, GL_NOTEQUAL>,
		EnumValuePair<EnumType::GREATER_EQUAL, GL_GEQUAL>,
		EnumValuePair<EnumType::ALWAYS, GL_ALWAYS>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EBlendingEquation, GLenum>
{
	using EnumType = baregl::types::EBlendingEquation;
	using type = std::tuple<
		EnumValuePair<EnumType::FUNC_ADD, GL_FUNC_ADD>,
		EnumValuePair<EnumType::FUNC_SUBTRACT, GL_FUNC_SUBTRACT>,
		EnumValuePair<EnumType::FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT>,
		EnumValuePair<EnumType::MIN, GL_MIN>,
		EnumValuePair<EnumType::MAX, GL_MAX>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EBlendingFactor, GLenum>
{
	using EnumType = baregl::types::EBlendingFactor;
	using type = std::tuple<
		EnumValuePair<EnumType::ZERO, GL_ZERO>,
		EnumValuePair<EnumType::ONE, GL_ONE>,
		EnumValuePair<EnumType::SRC_COLOR, GL_SRC_COLOR>,
		EnumValuePair<EnumType::ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR>,
		EnumValuePair<EnumType::DST_COLOR, GL_DST_COLOR>,
		EnumValuePair<EnumType::ONE_MINUS_DST_COLOR, GL_ONE_MINUS_DST_COLOR>,
		EnumValuePair<EnumType::SRC_ALPHA, GL_SRC_ALPHA>,
		EnumValuePair<EnumType::ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA>,
		EnumValuePair<EnumType::DST_ALPHA, GL_DST_ALPHA>,
		EnumValuePair<EnumType::ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA>,
		EnumValuePair<EnumType::CONSTANT_COLOR, GL_CONSTANT_COLOR>,
		EnumValuePair<EnumType::ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR>,
		EnumValuePair<EnumType::CONSTANT_ALPHA, GL_CONSTANT_ALPHA>,
		EnumValuePair<EnumType::ONE_MINUS_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA>,
		EnumValuePair<EnumType::SRC_ALPHA_SATURATE, GL_SRC_ALPHA_SATURATE>,
		EnumValuePair<EnumType::SRC1_COLOR, GL_SRC1_COLOR>,
		EnumValuePair<EnumType::ONE_MINUS_SRC1_COLOR, GL_ONE_MINUS_SRC1_COLOR>,
		EnumValuePair<EnumType::SRC1_ALPHA, GL_SRC1_ALPHA>,
		EnumValuePair<EnumType::ONE_MINUS_SRC1_ALPHA, GL_ONE_MINUS_SRC1_ALPHA>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ERasterizationMode, GLenum>
{
	using EnumType = baregl::types::ERasterizationMode;
	using type = std::tuple<
		EnumValuePair<EnumType::POINT, GL_POINT>,
		EnumValuePair<EnumType::LINE, GL_LINE>,
		EnumValuePair<EnumType::FILL, GL_FILL>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ECullFace, GLenum>
{
	using EnumType = baregl::types::ECullFace;
	using type = std::tuple<
		EnumValuePair<EnumType::FRONT, GL_FRONT>,
		EnumValuePair<EnumType::BACK, GL_BACK>,
		EnumValuePair<EnumType::FRONT_AND_BACK, GL_FRONT_AND_BACK>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EHint, GLenum>
{
	using EnumType = baregl::types::EHint;
	using type = std::tuple<
		EnumValuePair<EnumType::DONT_CARE, GL_DONT_CARE>,
		EnumValuePair<EnumType::FASTEST, GL_FASTEST>,
		EnumValuePair<EnumType::NICEST, GL_NICEST>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ELogicOperation, GLenum>
{
	using EnumType = baregl::types::ELogicOperation;
	using type = std::tuple<
		EnumValuePair<EnumType::CLEAR, GL_CLEAR>,
		EnumValuePair<EnumType::SET, GL_SET>,
		EnumValuePair<EnumType::COPY, GL_COPY>,
		EnumValuePair<EnumType::COPY_INVERTED, GL_COPY_INVERTED>,
		EnumValuePair<EnumType::NOOP, GL_NOOP>,
		EnumValuePair<EnumType::INVERT, GL_INVERT>,
		EnumValuePair<EnumType::AND, GL_AND>,
		EnumValuePair<EnumType::NAND, GL_NAND>,
		EnumValuePair<EnumType::OR, GL_OR>,
		EnumValuePair<EnumType::NOR, GL_NOR>,
		EnumValuePair<EnumType::XOR, GL_XOR>,
		EnumValuePair<EnumType::EQUIV, GL_EQUIV>,
		EnumValuePair<EnumType::AND_REVERSE, GL_AND_REVERSE>,
		EnumValuePair<EnumType::AND_INVERTED, GL_AND_INVERTED>,
		EnumValuePair<EnumType::OR_REVERSE, GL_OR_REVERSE>,
		EnumValuePair<EnumType::OR_INVERTED, GL_OR_INVERTED>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EProvokingVertexConvention, GLenum>
{
	using EnumType = baregl::types::EProvokingVertexConvention;
	using type = std::tuple<
		EnumValuePair<EnumType::PROVOKING_VERTEX, GL_PROVOKING_VERTEX>,
		EnumValuePair<EnumType::FIRST_VERTEX_CONVENTION, GL_FIRST_VERTEX_CONVENTION>,
		EnumValuePair<EnumType::LAST_VERTEX_CONVENTION, GL_LAST_VERTEX_CONVENTION>,
		EnumValuePair<EnumType::UNDEFINED_VERTEX, GL_UNDEFINED_VERTEX>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EDrawBuffer, GLenum>
{
	using EnumType = baregl::types::EDrawBuffer;
	using type = std::tuple<
		EnumValuePair<EnumType::NONE, GL_NONE>,
		EnumValuePair<EnumType::FRONT_LEFT, GL_FRONT_LEFT>,
		EnumValuePair<EnumType::FRONT_RIGHT, GL_FRONT_RIGHT>,
		EnumValuePair<EnumType::BACK_LEFT, GL_BACK_LEFT>,
		EnumValuePair<EnumType::BACK_RIGHT, GL_BACK_RIGHT>,
		EnumValuePair<EnumType::FRONT, GL_FRONT>,
		EnumValuePair<EnumType::BACK, GL_BACK>,
		EnumValuePair<EnumType::LEFT, GL_LEFT>,
		EnumValuePair<EnumType::RIGHT, GL_RIGHT>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT1>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT2>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT3>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT4>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT5>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT6>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT7>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT8>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT9>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT10>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT11>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT12>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT13>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT14>,
		EnumValuePair<EnumType::COLOR_ATTACHMENT15, GL_COLOR_ATTACHMENT15>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EOperation, GLenum>
{
	using EnumType = baregl::types::EOperation;
	using type = std::tuple<
		EnumValuePair<EnumType::KEEP, GL_KEEP>,
		EnumValuePair<EnumType::ZERO, GL_ZERO>,
		EnumValuePair<EnumType::REPLACE, GL_REPLACE>,
		EnumValuePair<EnumType::INCREMENT, GL_INCR>,
		EnumValuePair<EnumType::INCREMENT_WRAP, GL_INCR_WRAP>,
		EnumValuePair<EnumType::DECREMENT, GL_DECR>,
		EnumValuePair<EnumType::DECREMENT_WRAP, GL_DECR_WRAP>,
		EnumValuePair<EnumType::INVERT, GL_INVERT>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ERenderingCapability, GLenum>
{
	using EnumType = baregl::types::ERenderingCapability;
	using type = std::tuple<
		EnumValuePair<EnumType::BLEND, GL_BLEND>,
		EnumValuePair<EnumType::CULL_FACE, GL_CULL_FACE>,
		EnumValuePair<EnumType::DEPTH_TEST, GL_DEPTH_TEST>,
		EnumValuePair<EnumType::DITHER, GL_DITHER>,
		EnumValuePair<EnumType::POLYGON_OFFSET_FILL, GL_POLYGON_OFFSET_FILL>,
		EnumValuePair<EnumType::SAMPLE_ALPHA_TO_COVERAGE, GL_SAMPLE_ALPHA_TO_COVERAGE>,
		EnumValuePair<EnumType::SAMPLE_COVERAGE, GL_SAMPLE_COVERAGE>,
		EnumValuePair<EnumType::SCISSOR_TEST, GL_SCISSOR_TEST>,
		EnumValuePair<EnumType::STENCIL_TEST, GL_STENCIL_TEST>,
		EnumValuePair<EnumType::MULTISAMPLE, GL_MULTISAMPLE>,
		EnumValuePair<EnumType::LINE_SMOOTH, GL_LINE_SMOOTH>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EPrimitiveMode, GLenum>
{
	using EnumType = baregl::types::EPrimitiveMode;
	using type = std::tuple<
		EnumValuePair<EnumType::POINTS, GL_POINTS>,
		EnumValuePair<EnumType::LINES, GL_LINES>,
		EnumValuePair<EnumType::LINE_LOOP, GL_LINE_LOOP>,
		EnumValuePair<EnumType::LINE_STRIP, GL_LINE_STRIP>,
		EnumValuePair<EnumType::TRIANGLES, GL_TRIANGLES>,
		EnumValuePair<EnumType::TRIANGLE_STRIP, GL_TRIANGLE_STRIP>,
		EnumValuePair<EnumType::TRIANGLE_FAN, GL_TRIANGLE_FAN>,
		EnumValuePair<EnumType::LINES_ADJACENCY, GL_LINES_ADJACENCY>,
		EnumValuePair<EnumType::LINE_STRIP_ADJACENCY, GL_LINE_STRIP_ADJACENCY>,
		EnumValuePair<EnumType::PATCHES, GL_PATCHES>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EFormat, GLenum>
{
	using EnumType = baregl::types::EFormat;
	using type = std::tuple<
		EnumValuePair<EnumType::RED, GL_RED>,
		EnumValuePair<EnumType::RG, GL_RG>,
		EnumValuePair<EnumType::RGB, GL_RGB>,
		EnumValuePair<EnumType::BGR, GL_BGR>,
		EnumValuePair<EnumType::RGBA, GL_RGBA>,
		EnumValuePair<EnumType::BGRA, GL_BGRA>,
		EnumValuePair<EnumType::RED_INTEGER, GL_RED_INTEGER>,
		EnumValuePair<EnumType::RG_INTEGER, GL_RG_INTEGER>,
		EnumValuePair<EnumType::RGB_INTEGER, GL_RGB_INTEGER>,
		EnumValuePair<EnumType::BGR_INTEGER, GL_BGR_INTEGER>,
		EnumValuePair<EnumType::RGBA_INTEGER, GL_RGBA_INTEGER>,
		EnumValuePair<EnumType::BGRA_INTEGER, GL_BGRA_INTEGER>,
		EnumValuePair<EnumType::STENCIL_INDEX, GL_STENCIL_INDEX>,
		EnumValuePair<EnumType::DEPTH_COMPONENT, GL_DEPTH_COMPONENT>,
		EnumValuePair<EnumType::DEPTH_STENCIL, GL_DEPTH_STENCIL>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ETextureFilteringMode, GLenum>
{
	using EnumType = baregl::types::ETextureFilteringMode;
	using type = std::tuple<
		EnumValuePair<EnumType::NEAREST, GL_NEAREST>,
		EnumValuePair<EnumType::LINEAR, GL_LINEAR>,
		EnumValuePair<EnumType::NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST>,
		EnumValuePair<EnumType::LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR>,
		EnumValuePair<EnumType::LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST>,
		EnumValuePair<EnumType::NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ETextureWrapMode, GLenum>
{
	using EnumType = baregl::types::ETextureWrapMode;
	using type = std::tuple<
		EnumValuePair<EnumType::REPEAT, GL_REPEAT>,
		EnumValuePair<EnumType::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE>,
		EnumValuePair<EnumType::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER>,
		EnumValuePair<EnumType::MIRRORED_REPEAT, GL_MIRRORED_REPEAT>,
		EnumValuePair<EnumType::MIRROR_CLAMP_TO_EDGE, GL_MIRROR_CLAMP_TO_EDGE>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EPixelDataFormat, GLenum>
{
	using EnumType = baregl::types::EPixelDataFormat;
	using type = std::tuple<
		EnumValuePair<EnumType::STENCIL_INDEX, GL_STENCIL_INDEX>,
		EnumValuePair<EnumType::DEPTH_COMPONENT, GL_DEPTH_COMPONENT>,
		EnumValuePair<EnumType::RED, GL_RED>,
		EnumValuePair<EnumType::GREEN, GL_GREEN>,
		EnumValuePair<EnumType::BLUE, GL_BLUE>,
		EnumValuePair<EnumType::ALPHA, GL_ALPHA>,
		EnumValuePair<EnumType::RGB, GL_RGB>,
		EnumValuePair<EnumType::BGR, GL_BGR>,
		EnumValuePair<EnumType::RGBA, GL_RGBA>,
		EnumValuePair<EnumType::BGRA, GL_BGRA>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EPixelDataType, GLenum>
{
	using EnumType = baregl::types::EPixelDataType;
	using type = std::tuple<
		EnumValuePair<EnumType::BYTE, GL_BYTE>,
		EnumValuePair<EnumType::UNSIGNED_BYTE, GL_UNSIGNED_BYTE>,
		EnumValuePair<EnumType::SHORT, GL_SHORT>,
		EnumValuePair<EnumType::UNSIGNED_SHORT, GL_UNSIGNED_SHORT>,
		EnumValuePair<EnumType::INT, GL_INT>,
		EnumValuePair<EnumType::UNSIGNED_INT, GL_UNSIGNED_INT>,
		EnumValuePair<EnumType::FLOAT, GL_FLOAT>,
		EnumValuePair<EnumType::UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_3_3_2>,
		EnumValuePair<EnumType::UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_BYTE_2_3_3_REV>,
		EnumValuePair<EnumType::UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5>,
		EnumValuePair<EnumType::UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_5_6_5_REV>,
		EnumValuePair<EnumType::UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4>,
		EnumValuePair<EnumType::UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_4_4_4_4_REV>,
		EnumValuePair<EnumType::UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_5_5_5_1>,
		EnumValuePair<EnumType::UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_SHORT_1_5_5_5_REV>,
		EnumValuePair<EnumType::UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8>,
		EnumValuePair<EnumType::UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_8_8_8_8_REV>,
		EnumValuePair<EnumType::UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_10_10_10_2>,
		EnumValuePair<EnumType::UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EDataType, GLenum>
{
	using EnumType = baregl::types::EDataType;
	using type = std::tuple<
		EnumValuePair<EnumType::BYTE, GL_BYTE>,
		EnumValuePair<EnumType::UNSIGNED_BYTE, GL_UNSIGNED_BYTE>,
		EnumValuePair<EnumType::SHORT, GL_SHORT>,
		EnumValuePair<EnumType::UNSIGNED_SHORT, GL_UNSIGNED_SHORT>,
		EnumValuePair<EnumType::INT, GL_INT>,
		EnumValuePair<EnumType::UNSIGNED_INT, GL_UNSIGNED_INT>,
		EnumValuePair<EnumType::FLOAT, GL_FLOAT>,
		EnumValuePair<EnumType::DOUBLE, GL_DOUBLE>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EAccessSpecifier, GLenum>
{
	using EnumType = baregl::types::EAccessSpecifier;
	using type = std::tuple<
		EnumValuePair<EnumType::STREAM_DRAW, GL_STREAM_DRAW>,
		EnumValuePair<EnumType::STREAM_READ, GL_STREAM_READ>,
		EnumValuePair<EnumType::STREAM_COPY, GL_STREAM_COPY>,
		EnumValuePair<EnumType::DYNAMIC_DRAW, GL_DYNAMIC_DRAW>,
		EnumValuePair<EnumType::DYNAMIC_READ, GL_DYNAMIC_READ>,
		EnumValuePair<EnumType::DYNAMIC_COPY, GL_DYNAMIC_COPY>,
		EnumValuePair<EnumType::STATIC_DRAW, GL_STATIC_DRAW>,
		EnumValuePair<EnumType::STATIC_READ, GL_STATIC_READ>,
		EnumValuePair<EnumType::STATIC_COPY, GL_STATIC_COPY>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EImageAccessSpecifier, GLenum>
{
	using EnumType = baregl::types::EImageAccessSpecifier;
	using type = std::tuple<
		EnumValuePair<EnumType::READ_ONLY, GL_READ_ONLY>,
		EnumValuePair<EnumType::WRITE_ONLY, GL_WRITE_ONLY>,
		EnumValuePair<EnumType::READ_WRITE, GL_READ_WRITE>
		>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EUniformType, GLenum>
{
	using EnumType = baregl::types::EUniformType;
	using type = std::tuple<
		EnumValuePair<EnumType::BOOL, GL_BOOL>,
		EnumValuePair<EnumType::INT, GL_INT>,
		EnumValuePair<EnumType::UNSIGNED_INT, GL_UNSIGNED_INT>,
		EnumValuePair<EnumType::FLOAT, GL_FLOAT>,
		EnumValuePair<EnumType::FLOAT_VEC2, GL_FLOAT_VEC2>,
		EnumValuePair<EnumType::FLOAT_VEC3, GL_FLOAT_VEC3>,
		EnumValuePair<EnumType::FLOAT_VEC4, GL_FLOAT_VEC4>,
		EnumValuePair<EnumType::FLOAT_MAT3, GL_FLOAT_MAT3>,
		EnumValuePair<EnumType::FLOAT_MAT4, GL_FLOAT_MAT4>,
		EnumValuePair<EnumType::DOUBLE_MAT4, GL_DOUBLE_MAT4>,
		EnumValuePair<EnumType::SAMPLER_2D, GL_SAMPLER_2D>,
		EnumValuePair<EnumType::SAMPLER_CUBE, GL_SAMPLER_CUBE>,
		EnumValuePair<EnumType::IMAGE_2D, GL_IMAGE_2D>,
		EnumValuePair<EnumType::IMAGE_CUBE, GL_IMAGE_CUBE>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EInternalFormat, GLenum>
{
	using EnumType = baregl::types::EInternalFormat;
	using type = std::tuple <
		EnumValuePair<EnumType::DEPTH_COMPONENT, GL_DEPTH_COMPONENT>,
		EnumValuePair<EnumType::DEPTH_STENCIL, GL_DEPTH_STENCIL>,
		EnumValuePair<EnumType::RED, GL_RED>,
		EnumValuePair<EnumType::RG, GL_RG>,
		EnumValuePair<EnumType::RGB, GL_RGB>,
		EnumValuePair<EnumType::RGBA, GL_RGBA>,
		EnumValuePair<EnumType::R8, GL_R8>,
		EnumValuePair<EnumType::R8_SNORM, GL_R8_SNORM>,
		EnumValuePair<EnumType::R16, GL_R16>,
		EnumValuePair<EnumType::R16_SNORM, GL_R16_SNORM>,
		EnumValuePair<EnumType::RG8, GL_RG8>,
		EnumValuePair<EnumType::RG8_SNORM, GL_RG8_SNORM>,
		EnumValuePair<EnumType::RG16, GL_RG16>,
		EnumValuePair<EnumType::RG16_SNORM, GL_RG16_SNORM>,
		EnumValuePair<EnumType::R3_G3_B2, GL_R3_G3_B2>,
		EnumValuePair<EnumType::RGB4, GL_RGB4>,
		EnumValuePair<EnumType::RGB5, GL_RGB5>,
		EnumValuePair<EnumType::RGB8, GL_RGB8>,
		EnumValuePair<EnumType::RGB8_SNORM, GL_RGB8_SNORM>,
		EnumValuePair<EnumType::RGB10, GL_RGB10>,
		EnumValuePair<EnumType::RGB12, GL_RGB12>,
		EnumValuePair<EnumType::RGB16_SNORM, GL_RGB16_SNORM>,
		EnumValuePair<EnumType::RGBA2, GL_RGBA2>,
		EnumValuePair<EnumType::RGBA4, GL_RGBA4>,
		EnumValuePair<EnumType::RGB5_A1, GL_RGB5_A1>,
		EnumValuePair<EnumType::RGBA8, GL_RGBA8>,
		EnumValuePair<EnumType::RGBA8_SNORM, GL_RGBA8_SNORM>,
		EnumValuePair<EnumType::RGB10_A2, GL_RGB10_A2>,
		EnumValuePair<EnumType::RGB10_A2UI, GL_RGB10_A2UI>,
		EnumValuePair<EnumType::RGBA12, GL_RGBA12>,
		EnumValuePair<EnumType::RGBA16, GL_RGBA16>,
		EnumValuePair<EnumType::SRGB8, GL_SRGB8>,
		EnumValuePair<EnumType::SRGB8_ALPHA8, GL_SRGB8_ALPHA8>,
		EnumValuePair<EnumType::R16F, GL_R16F>,
		EnumValuePair<EnumType::RG16F, GL_RG16F>,
		EnumValuePair<EnumType::RGB16F, GL_RGB16F>,
		EnumValuePair<EnumType::RGBA16F, GL_RGBA16F>,
		EnumValuePair<EnumType::R32F, GL_R32F>,
		EnumValuePair<EnumType::RG32F, GL_RG32F>,
		EnumValuePair<EnumType::RGB32F, GL_RGB32F>,
		EnumValuePair<EnumType::RGBA32F, GL_RGBA32F>,
		EnumValuePair<EnumType::R11F_G11F_B10F, GL_R11F_G11F_B10F>,
		EnumValuePair<EnumType::RGB9_E5, GL_RGB9_E5>,
		EnumValuePair<EnumType::R8I, GL_R8I>,
		EnumValuePair<EnumType::R8UI, GL_R8UI>,
		EnumValuePair<EnumType::R16I, GL_R16I>,
		EnumValuePair<EnumType::R16UI, GL_R16UI>,
		EnumValuePair<EnumType::R32I, GL_R32I>,
		EnumValuePair<EnumType::R32UI, GL_R32UI>,
		EnumValuePair<EnumType::RG8I, GL_RG8I>,
		EnumValuePair<EnumType::RG8UI, GL_RG8UI>,
		EnumValuePair<EnumType::RG16I, GL_RG16I>,
		EnumValuePair<EnumType::RG16UI, GL_RG16UI>,
		EnumValuePair<EnumType::RG32I, GL_RG32I>,
		EnumValuePair<EnumType::RG32UI, GL_RG32UI>,
		EnumValuePair<EnumType::RGB8I, GL_RGB8I>,
		EnumValuePair<EnumType::RGB8UI, GL_RGB8UI>,
		EnumValuePair<EnumType::RGB16I, GL_RGB16I>,
		EnumValuePair<EnumType::RGB16UI, GL_RGB16UI>,
		EnumValuePair<EnumType::RGB32I, GL_RGB32I>,
		EnumValuePair<EnumType::RGB32UI, GL_RGB32UI>,
		EnumValuePair<EnumType::RGBA8I, GL_RGBA8I>,
		EnumValuePair<EnumType::RGBA8UI, GL_RGBA8UI>,
		EnumValuePair<EnumType::RGBA16I, GL_RGBA16I>,
		EnumValuePair<EnumType::RGBA16UI, GL_RGBA16UI>,
		EnumValuePair<EnumType::RGBA32I, GL_RGBA32I>,
		EnumValuePair<EnumType::RGBA32UI, GL_RGBA32UI>,
		EnumValuePair<EnumType::COMPRESSED_RED, GL_COMPRESSED_RED>,
		EnumValuePair<EnumType::COMPRESSED_RG, GL_COMPRESSED_RG>,
		EnumValuePair<EnumType::COMPRESSED_RGB, GL_COMPRESSED_RGB>,
		EnumValuePair<EnumType::COMPRESSED_RGBA, GL_COMPRESSED_RGBA>,
		EnumValuePair<EnumType::COMPRESSED_SRGB, GL_COMPRESSED_SRGB>,
		EnumValuePair<EnumType::COMPRESSED_SRGB_ALPHA, GL_COMPRESSED_SRGB_ALPHA>,
		EnumValuePair<EnumType::COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1>,
		EnumValuePair<EnumType::COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1>,
		EnumValuePair<EnumType::COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2>,
		EnumValuePair<EnumType::COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2>,
		EnumValuePair<EnumType::COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_RGBA_BPTC_UNORM>,
		EnumValuePair<EnumType::COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM>,
		EnumValuePair<EnumType::COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT>,
		EnumValuePair<EnumType::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT>,
		EnumValuePair<EnumType::COMPRESSED_RGB8_ETC2, GL_COMPRESSED_RGB8_ETC2>,
		EnumValuePair<EnumType::COMPRESSED_SRGB8_ETC2, GL_COMPRESSED_SRGB8_ETC2>,
		EnumValuePair<EnumType::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2>,
		EnumValuePair<EnumType::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2>,
		EnumValuePair<EnumType::COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_RGBA8_ETC2_EAC>,
		EnumValuePair<EnumType::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC>,
		EnumValuePair<EnumType::COMPRESSED_R11_EAC, GL_COMPRESSED_R11_EAC>,
		EnumValuePair<EnumType::COMPRESSED_SIGNED_R11_EAC, GL_COMPRESSED_SIGNED_R11_EAC>,
		EnumValuePair<EnumType::COMPRESSED_RG11_EAC, GL_COMPRESSED_RG11_EAC>,
		EnumValuePair<EnumType::COMPRESSED_SIGNED_RG11_EAC, GL_COMPRESSED_SIGNED_RG11_EAC>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EShaderType, GLenum>
{
	using EnumType = baregl::types::EShaderType;
	using type = std::tuple<
		EnumValuePair<EnumType::VERTEX, GL_VERTEX_SHADER>,
		EnumValuePair<EnumType::FRAGMENT, GL_FRAGMENT_SHADER>,
		EnumValuePair<EnumType::GEOMETRY, GL_GEOMETRY_SHADER>,
		EnumValuePair<EnumType::COMPUTE, GL_COMPUTE_SHADER>,
		EnumValuePair<EnumType::TESSELLATION_CONTROL, GL_TESS_CONTROL_SHADER>,
		EnumValuePair<EnumType::TESSELLATION_EVALUATION, GL_TESS_EVALUATION_SHADER>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EFramebufferAttachment, GLenum>
{
	using EnumType = baregl::types::EFramebufferAttachment;
	using type = std::tuple<
		EnumValuePair<EnumType::COLOR, GL_COLOR_ATTACHMENT0>,
		EnumValuePair<EnumType::DEPTH, GL_DEPTH_ATTACHMENT>,
		EnumValuePair<EnumType::STENCIL, GL_STENCIL_ATTACHMENT>,
		EnumValuePair<EnumType::DEPTH_STENCIL, GL_DEPTH_STENCIL_ATTACHMENT>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EBufferType, GLenum>
{
	using EnumType = baregl::types::EBufferType;
	using type = std::tuple<
		EnumValuePair<EnumType::VERTEX, GL_ARRAY_BUFFER>,
		EnumValuePair<EnumType::INDEX, GL_ELEMENT_ARRAY_BUFFER>,
		EnumValuePair<EnumType::UNIFORM, GL_UNIFORM_BUFFER>,
		EnumValuePair<EnumType::SHADER_STORAGE, GL_SHADER_STORAGE_BUFFER>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ETextureType, GLenum>
{
	using EnumType = baregl::types::ETextureType;
	using type = std::tuple<
		EnumValuePair<EnumType::TEXTURE_2D, GL_TEXTURE_2D>,
		EnumValuePair<EnumType::TEXTURE_CUBE, GL_TEXTURE_CUBE_MAP>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::ETextureUnit, GLenum>
{
	using EnumType = baregl::types::ETextureUnit;
	using type = std::tuple<
		EnumValuePair<EnumType::TEXTURE0,  GL_TEXTURE0>,
		EnumValuePair<EnumType::TEXTURE1,  GL_TEXTURE1>,
		EnumValuePair<EnumType::TEXTURE2,  GL_TEXTURE2>,
		EnumValuePair<EnumType::TEXTURE3,  GL_TEXTURE3>,
		EnumValuePair<EnumType::TEXTURE4,  GL_TEXTURE4>,
		EnumValuePair<EnumType::TEXTURE5,  GL_TEXTURE5>,
		EnumValuePair<EnumType::TEXTURE6,  GL_TEXTURE6>,
		EnumValuePair<EnumType::TEXTURE7,  GL_TEXTURE7>,
		EnumValuePair<EnumType::TEXTURE8,  GL_TEXTURE8>,
		EnumValuePair<EnumType::TEXTURE9,  GL_TEXTURE9>,
		EnumValuePair<EnumType::TEXTURE10, GL_TEXTURE10>,
		EnumValuePair<EnumType::TEXTURE11, GL_TEXTURE11>,
		EnumValuePair<EnumType::TEXTURE12, GL_TEXTURE12>,
		EnumValuePair<EnumType::TEXTURE13, GL_TEXTURE13>,
		EnumValuePair<EnumType::TEXTURE14, GL_TEXTURE14>,
		EnumValuePair<EnumType::TEXTURE15, GL_TEXTURE15>,
		EnumValuePair<EnumType::TEXTURE16, GL_TEXTURE16>,
		EnumValuePair<EnumType::TEXTURE17, GL_TEXTURE17>,
		EnumValuePair<EnumType::TEXTURE18, GL_TEXTURE18>,
		EnumValuePair<EnumType::TEXTURE19, GL_TEXTURE19>,
		EnumValuePair<EnumType::TEXTURE20, GL_TEXTURE20>,
		EnumValuePair<EnumType::TEXTURE21, GL_TEXTURE21>,
		EnumValuePair<EnumType::TEXTURE22, GL_TEXTURE22>,
		EnumValuePair<EnumType::TEXTURE23, GL_TEXTURE23>,
		EnumValuePair<EnumType::TEXTURE24, GL_TEXTURE24>,
		EnumValuePair<EnumType::TEXTURE25, GL_TEXTURE25>,
		EnumValuePair<EnumType::TEXTURE26, GL_TEXTURE26>,
		EnumValuePair<EnumType::TEXTURE27, GL_TEXTURE27>,
		EnumValuePair<EnumType::TEXTURE28, GL_TEXTURE28>,
		EnumValuePair<EnumType::TEXTURE29, GL_TEXTURE29>,
		EnumValuePair<EnumType::TEXTURE30, GL_TEXTURE30>,
		EnumValuePair<EnumType::TEXTURE31, GL_TEXTURE31>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EMemoryBarrierFlags, GLbitfield>
{
	using EnumType = baregl::types::EMemoryBarrierFlags;
	using type = std::tuple<
		EnumValuePair<EnumType::VERTEX_ATTRIB_ARRAY, GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT>,
		EnumValuePair<EnumType::ELEMENT_ARRAY, GL_ELEMENT_ARRAY_BARRIER_BIT>,
		EnumValuePair<EnumType::UNIFORM, GL_UNIFORM_BARRIER_BIT>,
		EnumValuePair<EnumType::TEXTURE_FETCH, GL_TEXTURE_FETCH_BARRIER_BIT>,
		EnumValuePair<EnumType::SHADER_IMAGE_ACCESS, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT>,
		EnumValuePair<EnumType::COMMAND, GL_COMMAND_BARRIER_BIT>,
		EnumValuePair<EnumType::PIXEL_BUFFER, GL_PIXEL_BUFFER_BARRIER_BIT>,
		EnumValuePair<EnumType::TEXTURE_UPDATE, GL_TEXTURE_UPDATE_BARRIER_BIT>,
		EnumValuePair<EnumType::BUFFER_UPDATE, GL_BUFFER_UPDATE_BARRIER_BIT>,
		EnumValuePair<EnumType::CLIENT_MAPPED_BUFFER, GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT>,
		EnumValuePair<EnumType::FRAMEBUFFER, GL_FRAMEBUFFER_BARRIER_BIT>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK, GL_TRANSFORM_FEEDBACK_BARRIER_BIT>,
		EnumValuePair<EnumType::ATOMIC_COUNTER, GL_ATOMIC_COUNTER_BARRIER_BIT>,
		EnumValuePair<EnumType::SHADER_STORAGE, GL_SHADER_STORAGE_BARRIER_BIT>,
		EnumValuePair<EnumType::QUERY_BUFFER, GL_QUERY_BUFFER_BARRIER_BIT>,
		EnumValuePair<EnumType::ALL, GL_ALL_BARRIER_BITS>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EContextFlags, GLbitfield>
{
	using EnumType = baregl::types::EContextFlags;
	using type = std::tuple<
		EnumValuePair<EnumType::NONE, static_cast<GLbitfield>(0)>,
		EnumValuePair<EnumType::FORWARD_COMPATIBLE, GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT>,
		EnumValuePair<EnumType::DEBUG_BIT, GL_CONTEXT_FLAG_DEBUG_BIT>,
		EnumValuePair<EnumType::ROBUST_ACCESS, GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT>,
		EnumValuePair<EnumType::NO_ERROR, static_cast<GLbitfield>(0x00000008)>
	>;
};

template <>
struct baregl::utils::MappingFor<baregl::types::EGetParameter, GLenum>
{
	using EnumType = baregl::types::EGetParameter;
	using type = std::tuple<
		EnumValuePair<EnumType::PATCH_VERTICES, GL_PATCH_VERTICES>,
		EnumValuePair<EnumType::PATCH_DEFAULT_OUTER_LEVEL, GL_PATCH_DEFAULT_OUTER_LEVEL>,
		EnumValuePair<EnumType::PATCH_DEFAULT_INNER_LEVEL, GL_PATCH_DEFAULT_INNER_LEVEL>,
		EnumValuePair<EnumType::ELEMENT_ARRAY_BUFFER_BINDING, GL_ELEMENT_ARRAY_BUFFER_BINDING>,
		EnumValuePair<EnumType::VERTEX_BINDING_OFFSET, GL_VERTEX_BINDING_OFFSET>,
		EnumValuePair<EnumType::VERTEX_BINDING_STRIDE, GL_VERTEX_BINDING_STRIDE>,
		EnumValuePair<EnumType::VERTEX_BINDING_DIVISOR, GL_VERTEX_BINDING_DIVISOR>,
		EnumValuePair<EnumType::VERTEX_BINDING_BUFFER, GL_VERTEX_BINDING_BUFFER>,
		EnumValuePair<EnumType::ARRAY_BUFFER_BINDING, GL_ARRAY_BUFFER_BINDING>,
		EnumValuePair<EnumType::DRAW_INDIRECT_BUFFER_BINDING, GL_DRAW_INDIRECT_BUFFER_BINDING>,
		EnumValuePair<EnumType::VERTEX_ARRAY_BINDING, GL_VERTEX_ARRAY_BINDING>,
		EnumValuePair<EnumType::PRIMITIVE_RESTART, GL_PRIMITIVE_RESTART>,
		EnumValuePair<EnumType::PRIMITIVE_RESTART_FIXED_INDEX, GL_PRIMITIVE_RESTART_FIXED_INDEX>,
		EnumValuePair<EnumType::PRIMITIVE_RESTART_INDEX, GL_PRIMITIVE_RESTART_INDEX>,
		EnumValuePair<EnumType::VIEWPORT, GL_VIEWPORT>,
		EnumValuePair<EnumType::DEPTH_RANGE, GL_DEPTH_RANGE>,
		EnumValuePair<EnumType::CLIP_DISTANCE0, GL_CLIP_DISTANCE0>,
		EnumValuePair<EnumType::CLIP_DISTANCE1, GL_CLIP_DISTANCE1>,
		EnumValuePair<EnumType::CLIP_DISTANCE2, GL_CLIP_DISTANCE2>,
		EnumValuePair<EnumType::CLIP_DISTANCE3, GL_CLIP_DISTANCE3>,
		EnumValuePair<EnumType::CLIP_DISTANCE4, GL_CLIP_DISTANCE4>,
		EnumValuePair<EnumType::CLIP_DISTANCE5, GL_CLIP_DISTANCE5>,
		EnumValuePair<EnumType::CLIP_DISTANCE6, GL_CLIP_DISTANCE6>,
		EnumValuePair<EnumType::CLIP_DISTANCE7, GL_CLIP_DISTANCE7>,
		EnumValuePair<EnumType::CLIP_ORIGIN, GL_CLIP_ORIGIN>,
		EnumValuePair<EnumType::CLIP_DEPTH_MODE, GL_CLIP_DEPTH_MODE>,
		EnumValuePair<EnumType::DEPTH_CLAMP, GL_DEPTH_CLAMP>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK_BINDING, GL_TRANSFORM_FEEDBACK_BINDING>,
		EnumValuePair<EnumType::CLAMP_READ_COLOR, GL_CLAMP_READ_COLOR>,
		EnumValuePair<EnumType::PROVOKING_VERTEX, GL_PROVOKING_VERTEX>,
		EnumValuePair<EnumType::RASTERIZER_DISCARD, GL_RASTERIZER_DISCARD>,
		EnumValuePair<EnumType::POINT_SIZE, GL_POINT_SIZE>,
		EnumValuePair<EnumType::POINT_FADE_THRESHOLD_SIZE, GL_POINT_FADE_THRESHOLD_SIZE>,
		EnumValuePair<EnumType::POINT_SPRITE_COORD_ORIGIN, GL_POINT_SPRITE_COORD_ORIGIN>,
		EnumValuePair<EnumType::LINE_WIDTH, GL_LINE_WIDTH>,
		EnumValuePair<EnumType::LINE_SMOOTH, GL_LINE_SMOOTH>,
		EnumValuePair<EnumType::CULL_FACE, GL_CULL_FACE>,
		EnumValuePair<EnumType::CULL_FACE_MODE, GL_CULL_FACE_MODE>,
		EnumValuePair<EnumType::FRONT_FACE, GL_FRONT_FACE>,
		EnumValuePair<EnumType::POLYGON_SMOOTH, GL_POLYGON_SMOOTH>,
		EnumValuePair<EnumType::POLYGON_MODE, GL_POLYGON_MODE>,
		EnumValuePair<EnumType::POLYGON_OFFSET_FACTOR, GL_POLYGON_OFFSET_FACTOR>,
		EnumValuePair<EnumType::POLYGON_OFFSET_UNITS, GL_POLYGON_OFFSET_UNITS>,
		EnumValuePair<EnumType::POLYGON_OFFSET_POINT, GL_POLYGON_OFFSET_POINT>,
		EnumValuePair<EnumType::POLYGON_OFFSET_LINE, GL_POLYGON_OFFSET_LINE>,
		EnumValuePair<EnumType::POLYGON_OFFSET_FILL, GL_POLYGON_OFFSET_FILL>,
		EnumValuePair<EnumType::MULTISAMPLE, GL_MULTISAMPLE>,
		EnumValuePair<EnumType::SAMPLE_ALPHA_TO_COVERAGE, GL_SAMPLE_ALPHA_TO_COVERAGE>,
		EnumValuePair<EnumType::SAMPLE_ALPHA_TO_ONE, GL_SAMPLE_ALPHA_TO_ONE>,
		EnumValuePair<EnumType::SAMPLE_COVERAGE, GL_SAMPLE_COVERAGE>,
		EnumValuePair<EnumType::SAMPLE_COVERAGE_VALUE, GL_SAMPLE_COVERAGE_VALUE>,
		EnumValuePair<EnumType::SAMPLE_COVERAGE_INVERT, GL_SAMPLE_COVERAGE_INVERT>,
		EnumValuePair<EnumType::SAMPLE_SHADING, GL_SAMPLE_SHADING>,
		EnumValuePair<EnumType::MIN_SAMPLE_SHADING_VALUE, GL_MIN_SAMPLE_SHADING_VALUE>,
		EnumValuePair<EnumType::SAMPLE_MASK, GL_SAMPLE_MASK>,
		EnumValuePair<EnumType::SAMPLE_MASK_VALUE, GL_SAMPLE_MASK_VALUE>,
		EnumValuePair<EnumType::TEXTURE_BINDING_1D, GL_TEXTURE_BINDING_1D>,
		EnumValuePair<EnumType::TEXTURE_BINDING_2D, GL_TEXTURE_BINDING_2D>,
		EnumValuePair<EnumType::TEXTURE_BINDING_3D, GL_TEXTURE_BINDING_3D>,
		EnumValuePair<EnumType::TEXTURE_BINDING_1D_ARRAY, GL_TEXTURE_BINDING_1D_ARRAY>,
		EnumValuePair<EnumType::TEXTURE_BINDING_2D_ARRAY, GL_TEXTURE_BINDING_2D_ARRAY>,
		EnumValuePair<EnumType::TEXTURE_BINDING_CUBE_MAP_ARRAY, GL_TEXTURE_BINDING_CUBE_MAP_ARRAY>,
		EnumValuePair<EnumType::TEXTURE_BINDING_RECTANGLE, GL_TEXTURE_BINDING_RECTANGLE>,
		EnumValuePair<EnumType::TEXTURE_BINDING_BUFFER, GL_TEXTURE_BINDING_BUFFER>,
		EnumValuePair<EnumType::TEXTURE_BINDING_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP>,
		EnumValuePair<EnumType::TEXTURE_BINDING_2D_MULTISAMPLE, GL_TEXTURE_BINDING_2D_MULTISAMPLE>,
		EnumValuePair<EnumType::TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY>,
		EnumValuePair<EnumType::SAMPLER_BINDING, GL_SAMPLER_BINDING>,
		EnumValuePair<EnumType::ACTIVE_TEXTURE, GL_ACTIVE_TEXTURE>,
		EnumValuePair<EnumType::SCISSOR_TEST, GL_SCISSOR_TEST>,
		EnumValuePair<EnumType::SCISSOR_BOX, GL_SCISSOR_BOX>,
		EnumValuePair<EnumType::STENCIL_TEST, GL_STENCIL_TEST>,
		EnumValuePair<EnumType::STENCIL_FUNC, GL_STENCIL_FUNC>,
		EnumValuePair<EnumType::STENCIL_VALUE_MASK, GL_STENCIL_VALUE_MASK>,
		EnumValuePair<EnumType::STENCIL_REF, GL_STENCIL_REF>,
		EnumValuePair<EnumType::STENCIL_FAIL, GL_STENCIL_FAIL>,
		EnumValuePair<EnumType::STENCIL_PASS_DEPTH_FAIL, GL_STENCIL_PASS_DEPTH_FAIL>,
		EnumValuePair<EnumType::STENCIL_PASS_DEPTH_PASS, GL_STENCIL_PASS_DEPTH_PASS>,
		EnumValuePair<EnumType::STENCIL_BACK_FUNC, GL_STENCIL_BACK_FUNC>,
		EnumValuePair<EnumType::STENCIL_BACK_VALUE_MASK, GL_STENCIL_BACK_VALUE_MASK>,
		EnumValuePair<EnumType::STENCIL_BACK_REF, GL_STENCIL_BACK_REF>,
		EnumValuePair<EnumType::STENCIL_BACK_FAIL, GL_STENCIL_BACK_FAIL>,
		EnumValuePair<EnumType::STENCIL_BACK_PASS_DEPTH_FAIL, GL_STENCIL_BACK_PASS_DEPTH_FAIL>,
		EnumValuePair<EnumType::STENCIL_BACK_PASS_DEPTH_PASS, GL_STENCIL_BACK_PASS_DEPTH_PASS>,
		EnumValuePair<EnumType::DEPTH_TEST, GL_DEPTH_TEST>,
		EnumValuePair<EnumType::DEPTH_FUNC, GL_DEPTH_FUNC>,
		EnumValuePair<EnumType::BLEND, GL_BLEND>,
		EnumValuePair<EnumType::BLEND_SRC_RGB, GL_BLEND_SRC_RGB>,
		EnumValuePair<EnumType::BLEND_SRC_ALPHA, GL_BLEND_SRC_ALPHA>,
		EnumValuePair<EnumType::BLEND_DST_RGB, GL_BLEND_DST_RGB>,
		EnumValuePair<EnumType::BLEND_DST_ALPHA, GL_BLEND_DST_ALPHA>,
		EnumValuePair<EnumType::BLEND_EQUATION_RGB, GL_BLEND_EQUATION_RGB>,
		EnumValuePair<EnumType::BLEND_EQUATION_ALPHA, GL_BLEND_EQUATION_ALPHA>,
		EnumValuePair<EnumType::BLEND_COLOR, GL_BLEND_COLOR>,
		EnumValuePair<EnumType::FRAMEBUFFER_SRGB, GL_FRAMEBUFFER_SRGB>,
		EnumValuePair<EnumType::DITHER, GL_DITHER>,
		EnumValuePair<EnumType::COLOR_LOGIC_OP, GL_COLOR_LOGIC_OP>,
		EnumValuePair<EnumType::LOGIC_OP_MODE, GL_LOGIC_OP_MODE>,
		EnumValuePair<EnumType::COLOR_WRITEMASK, GL_COLOR_WRITEMASK>,
		EnumValuePair<EnumType::DEPTH_WRITEMASK, GL_DEPTH_WRITEMASK>,
		EnumValuePair<EnumType::STENCIL_WRITEMASK, GL_STENCIL_WRITEMASK>,
		EnumValuePair<EnumType::STENCIL_BACK_WRITEMASK, GL_STENCIL_BACK_WRITEMASK>,
		EnumValuePair<EnumType::COLOR_CLEAR_VALUE, GL_COLOR_CLEAR_VALUE>,
		EnumValuePair<EnumType::DEPTH_CLEAR_VALUE, GL_DEPTH_CLEAR_VALUE>,
		EnumValuePair<EnumType::STENCIL_CLEAR_VALUE, GL_STENCIL_CLEAR_VALUE>,
		EnumValuePair<EnumType::DRAW_FRAMEBUFFER_BINDING, GL_DRAW_FRAMEBUFFER_BINDING>,
		EnumValuePair<EnumType::READ_FRAMEBUFFER_BINDING, GL_READ_FRAMEBUFFER_BINDING>,
		EnumValuePair<EnumType::DRAW_BUFFER, GL_DRAW_BUFFER>,
		EnumValuePair<EnumType::DRAW_BUFFER0, GL_DRAW_BUFFER0>,
		EnumValuePair<EnumType::DRAW_BUFFER1, GL_DRAW_BUFFER1>,
		EnumValuePair<EnumType::DRAW_BUFFER2, GL_DRAW_BUFFER2>,
		EnumValuePair<EnumType::DRAW_BUFFER3, GL_DRAW_BUFFER3>,
		EnumValuePair<EnumType::DRAW_BUFFER4, GL_DRAW_BUFFER4>,
		EnumValuePair<EnumType::DRAW_BUFFER5, GL_DRAW_BUFFER5>,
		EnumValuePair<EnumType::DRAW_BUFFER6, GL_DRAW_BUFFER6>,
		EnumValuePair<EnumType::DRAW_BUFFER7, GL_DRAW_BUFFER7>,
		EnumValuePair<EnumType::DRAW_BUFFER8, GL_DRAW_BUFFER8>,
		EnumValuePair<EnumType::DRAW_BUFFER9, GL_DRAW_BUFFER9>,
		EnumValuePair<EnumType::DRAW_BUFFER10, GL_DRAW_BUFFER10>,
		EnumValuePair<EnumType::DRAW_BUFFER11, GL_DRAW_BUFFER11>,
		EnumValuePair<EnumType::DRAW_BUFFER12, GL_DRAW_BUFFER12>,
		EnumValuePair<EnumType::DRAW_BUFFER13, GL_DRAW_BUFFER13>,
		EnumValuePair<EnumType::DRAW_BUFFER14, GL_DRAW_BUFFER14>,
		EnumValuePair<EnumType::DRAW_BUFFER15, GL_DRAW_BUFFER15>,
		EnumValuePair<EnumType::READ_BUFFER, GL_READ_BUFFER>,
		EnumValuePair<EnumType::RENDERBUFFER_BINDING, GL_RENDERBUFFER_BINDING>,
		EnumValuePair<EnumType::UNPACK_SWAP_BYTES, GL_UNPACK_SWAP_BYTES>,
		EnumValuePair<EnumType::UNPACK_LSB_FIRST, GL_UNPACK_LSB_FIRST>,
		EnumValuePair<EnumType::UNPACK_IMAGE_HEIGHT, GL_UNPACK_IMAGE_HEIGHT>,
		EnumValuePair<EnumType::UNPACK_SKIP_IMAGES, GL_UNPACK_SKIP_IMAGES>,
		EnumValuePair<EnumType::UNPACK_ROW_LENGTH, GL_UNPACK_ROW_LENGTH>,
		EnumValuePair<EnumType::UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_ROWS>,
		EnumValuePair<EnumType::UNPACK_SKIP_PIXELS, GL_UNPACK_SKIP_PIXELS>,
		EnumValuePair<EnumType::UNPACK_ALIGNMENT, GL_UNPACK_ALIGNMENT>,
		EnumValuePair<EnumType::UNPACK_COMPRESSED_BLOCK_WIDTH, GL_UNPACK_COMPRESSED_BLOCK_WIDTH>,
		EnumValuePair<EnumType::UNPACK_COMPRESSED_BLOCK_HEIGHT, GL_UNPACK_COMPRESSED_BLOCK_HEIGHT>,
		EnumValuePair<EnumType::UNPACK_COMPRESSED_BLOCK_DEPTH, GL_UNPACK_COMPRESSED_BLOCK_DEPTH>,
		EnumValuePair<EnumType::UNPACK_COMPRESSED_BLOCK_SIZE, GL_UNPACK_COMPRESSED_BLOCK_SIZE>,
		EnumValuePair<EnumType::PIXEL_UNPACK_BUFFER_BINDING, GL_PIXEL_UNPACK_BUFFER_BINDING>,
		EnumValuePair<EnumType::PACK_SWAP_BYTES, GL_PACK_SWAP_BYTES>,
		EnumValuePair<EnumType::PACK_LSB_FIRST, GL_PACK_LSB_FIRST>,
		EnumValuePair<EnumType::PACK_IMAGE_HEIGHT, GL_PACK_IMAGE_HEIGHT>,
		EnumValuePair<EnumType::PACK_SKIP_IMAGES, GL_PACK_SKIP_IMAGES>,
		EnumValuePair<EnumType::PACK_ROW_LENGTH, GL_PACK_ROW_LENGTH>,
		EnumValuePair<EnumType::PACK_SKIP_ROWS, GL_PACK_SKIP_ROWS>,
		EnumValuePair<EnumType::PACK_SKIP_PIXELS, GL_PACK_SKIP_PIXELS>,
		EnumValuePair<EnumType::PACK_ALIGNMENT, GL_PACK_ALIGNMENT>,
		EnumValuePair<EnumType::PACK_COMPRESSED_BLOCK_WIDTH, GL_PACK_COMPRESSED_BLOCK_WIDTH>,
		EnumValuePair<EnumType::PACK_COMPRESSED_BLOCK_HEIGHT, GL_PACK_COMPRESSED_BLOCK_HEIGHT>,
		EnumValuePair<EnumType::PACK_COMPRESSED_BLOCK_DEPTH, GL_PACK_COMPRESSED_BLOCK_DEPTH>,
		EnumValuePair<EnumType::PACK_COMPRESSED_BLOCK_SIZE, GL_PACK_COMPRESSED_BLOCK_SIZE>,
		EnumValuePair<EnumType::PIXEL_PACK_BUFFER_BINDING, GL_PIXEL_PACK_BUFFER_BINDING>,
		EnumValuePair<EnumType::CURRENT_PROGRAM, GL_CURRENT_PROGRAM>,
		EnumValuePair<EnumType::PROGRAM_PIPELINE_BINDING, GL_PROGRAM_PIPELINE_BINDING>,
		EnumValuePair<EnumType::PROGRAM_POINT_SIZE, GL_PROGRAM_POINT_SIZE>,
		EnumValuePair<EnumType::IMAGE_BINDING_NAME, GL_IMAGE_BINDING_NAME>,
		EnumValuePair<EnumType::IMAGE_BINDING_LEVEL, GL_IMAGE_BINDING_LEVEL>,
		EnumValuePair<EnumType::IMAGE_BINDING_LAYERED, GL_IMAGE_BINDING_LAYERED>,
		EnumValuePair<EnumType::IMAGE_BINDING_LAYER, GL_IMAGE_BINDING_LAYER>,
		EnumValuePair<EnumType::IMAGE_BINDING_ACCESS, GL_IMAGE_BINDING_ACCESS>,
		EnumValuePair<EnumType::IMAGE_BINDING_FORMAT, GL_IMAGE_BINDING_FORMAT>,
		EnumValuePair<EnumType::ATOMIC_COUNTER_BUFFER_BINDING, GL_ATOMIC_COUNTER_BUFFER_BINDING>,
		EnumValuePair<EnumType::ATOMIC_COUNTER_BUFFER_START, GL_ATOMIC_COUNTER_BUFFER_START>,
		EnumValuePair<EnumType::ATOMIC_COUNTER_BUFFER_SIZE, GL_ATOMIC_COUNTER_BUFFER_SIZE>,
		EnumValuePair<EnumType::SHADER_STORAGE_BUFFER_BINDING, GL_SHADER_STORAGE_BUFFER_BINDING>,
		EnumValuePair<EnumType::SHADER_STORAGE_BUFFER_START, GL_SHADER_STORAGE_BUFFER_START>,
		EnumValuePair<EnumType::SHADER_STORAGE_BUFFER_SIZE, GL_SHADER_STORAGE_BUFFER_SIZE>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK_BUFFER_BINDING, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK_BUFFER_START, GL_TRANSFORM_FEEDBACK_BUFFER_START>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK_BUFFER_SIZE, GL_TRANSFORM_FEEDBACK_BUFFER_SIZE>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK_PAUSED, GL_TRANSFORM_FEEDBACK_PAUSED>,
		EnumValuePair<EnumType::TRANSFORM_FEEDBACK_ACTIVE, GL_TRANSFORM_FEEDBACK_ACTIVE>,
		EnumValuePair<EnumType::UNIFORM_BUFFER_BINDING, GL_UNIFORM_BUFFER_BINDING>,
		EnumValuePair<EnumType::UNIFORM_BUFFER_START, GL_UNIFORM_BUFFER_START>,
		EnumValuePair<EnumType::UNIFORM_BUFFER_SIZE, GL_UNIFORM_BUFFER_SIZE>,
		EnumValuePair<EnumType::LINE_SMOOTH_HINT, GL_LINE_SMOOTH_HINT>,
		EnumValuePair<EnumType::POLYGON_SMOOTH_HINT, GL_POLYGON_SMOOTH_HINT>,
		EnumValuePair<EnumType::TEXTURE_COMPRESSION_HINT, GL_TEXTURE_COMPRESSION_HINT>,
		EnumValuePair<EnumType::FRAGMENT_SHADER_DERIVATIVE_HINT, GL_FRAGMENT_SHADER_DERIVATIVE_HINT>,
		EnumValuePair<EnumType::DISPATCH_INDIRECT_BUFFER_BINDING, GL_DISPATCH_INDIRECT_BUFFER_BINDING>,
		EnumValuePair<EnumType::CONTEXT_RELEASE_BEHAVIOR, GL_CONTEXT_RELEASE_BEHAVIOR>,
		EnumValuePair<EnumType::MAX_CLIP_DISTANCES, GL_MAX_CLIP_DISTANCES>,
		EnumValuePair<EnumType::MAX_CULL_DISTANCES, GL_MAX_CULL_DISTANCES>,
		EnumValuePair<EnumType::MAX_COMBINED_CLIP_AND_CULL_DISTANCES, GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES>,
		EnumValuePair<EnumType::SUBPIXEL_BITS, GL_SUBPIXEL_BITS>,
		EnumValuePair<EnumType::MAX_ELEMENT_INDEX, GL_MAX_ELEMENT_INDEX>,
		EnumValuePair<EnumType::PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED, GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED>,
		EnumValuePair<EnumType::MAX_3D_TEXTURE_SIZE, GL_MAX_3D_TEXTURE_SIZE>,
		EnumValuePair<EnumType::MAX_TEXTURE_SIZE, GL_MAX_TEXTURE_SIZE>,
		EnumValuePair<EnumType::MAX_ARRAY_TEXTURE_LAYERS, GL_MAX_ARRAY_TEXTURE_LAYERS>,
		EnumValuePair<EnumType::MAX_TEXTURE_LOD_BIAS, GL_MAX_TEXTURE_LOD_BIAS>,
		EnumValuePair<EnumType::MAX_CUBE_MAP_TEXTURE_SIZE, GL_MAX_CUBE_MAP_TEXTURE_SIZE>,
		EnumValuePair<EnumType::MAX_RENDERBUFFER_SIZE, GL_MAX_RENDERBUFFER_SIZE>,
		EnumValuePair<EnumType::MAX_VIEWPORT_DIMS, GL_MAX_VIEWPORT_DIMS>,
		EnumValuePair<EnumType::MAX_VIEWPORTS, GL_MAX_VIEWPORTS>,
		EnumValuePair<EnumType::VIEWPORT_SUBPIXEL_BITS, GL_VIEWPORT_SUBPIXEL_BITS>,
		EnumValuePair<EnumType::VIEWPORT_BOUNDS_RANGE, GL_VIEWPORT_BOUNDS_RANGE>,
		EnumValuePair<EnumType::LAYER_PROVOKING_VERTEX, GL_LAYER_PROVOKING_VERTEX>,
		EnumValuePair<EnumType::VIEWPORT_INDEX_PROVOKING_VERTEX, GL_VIEWPORT_INDEX_PROVOKING_VERTEX>,
		EnumValuePair<EnumType::POINT_SIZE_RANGE, GL_POINT_SIZE_RANGE>,
		EnumValuePair<EnumType::POINT_SIZE_GRANULARITY, GL_POINT_SIZE_GRANULARITY>,
		EnumValuePair<EnumType::ALIASED_LINE_WIDTH_RANGE, GL_ALIASED_LINE_WIDTH_RANGE>,
		EnumValuePair<EnumType::SMOOTH_LINE_WIDTH_RANGE, GL_SMOOTH_LINE_WIDTH_RANGE>,
		EnumValuePair<EnumType::SMOOTH_LINE_WIDTH_GRANULARITY, GL_SMOOTH_LINE_WIDTH_GRANULARITY>,
		EnumValuePair<EnumType::MAX_ELEMENTS_INDICES, GL_MAX_ELEMENTS_INDICES>,
		EnumValuePair<EnumType::MAX_ELEMENTS_VERTICES, GL_MAX_ELEMENTS_VERTICES>,
		EnumValuePair<EnumType::MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET>,
		EnumValuePair<EnumType::MAX_VERTEX_ATTRIB_BINDINGS, GL_MAX_VERTEX_ATTRIB_BINDINGS>,
		EnumValuePair<EnumType::MAX_VERTEX_ATTRIB_STRIDE, GL_MAX_VERTEX_ATTRIB_STRIDE>,
		EnumValuePair<EnumType::NUM_COMPRESSED_TEXTURE_FORMATS, GL_NUM_COMPRESSED_TEXTURE_FORMATS>,
		EnumValuePair<EnumType::COMPRESSED_TEXTURE_FORMATS, GL_COMPRESSED_TEXTURE_FORMATS>,
		EnumValuePair<EnumType::MAX_TEXTURE_BUFFER_SIZE, GL_MAX_TEXTURE_BUFFER_SIZE>,
		EnumValuePair<EnumType::MAX_RECTANGLE_TEXTURE_SIZE, GL_MAX_RECTANGLE_TEXTURE_SIZE>,
		EnumValuePair<EnumType::NUM_PROGRAM_BINARY_FORMATS, GL_NUM_PROGRAM_BINARY_FORMATS>,
		EnumValuePair<EnumType::PROGRAM_BINARY_FORMATS, GL_PROGRAM_BINARY_FORMATS>,
		EnumValuePair<EnumType::NUM_SHADER_BINARY_FORMATS, GL_NUM_SHADER_BINARY_FORMATS>,
		EnumValuePair<EnumType::SHADER_BINARY_FORMATS, GL_SHADER_BINARY_FORMATS>,
		EnumValuePair<EnumType::SHADER_COMPILER, GL_SHADER_COMPILER>,
		EnumValuePair<EnumType::MIN_MAP_BUFFER_ALIGNMENT, GL_MIN_MAP_BUFFER_ALIGNMENT>,
		EnumValuePair<EnumType::TEXTURE_BUFFER_OFFSET_ALIGNMENT, GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT>,
		EnumValuePair<EnumType::MAJOR_VERSION, GL_MAJOR_VERSION>,
		EnumValuePair<EnumType::MINOR_VERSION, GL_MINOR_VERSION>,
		EnumValuePair<EnumType::CONTEXT_FLAGS, GL_CONTEXT_FLAGS>,
		EnumValuePair<EnumType::CONTEXT_PROFILE_MASK, GL_CONTEXT_PROFILE_MASK>,
		EnumValuePair<EnumType::EXTENSIONS, GL_EXTENSIONS>,
		EnumValuePair<EnumType::NUM_EXTENSIONS, GL_NUM_EXTENSIONS>,
		EnumValuePair<EnumType::RENDERER, GL_RENDERER>,
		EnumValuePair<EnumType::SHADING_LANGUAGE_VERSION, GL_SHADING_LANGUAGE_VERSION>,
		EnumValuePair<EnumType::NUM_SHADING_LANGUAGE_VERSIONS, GL_NUM_SHADING_LANGUAGE_VERSIONS>,
		EnumValuePair<EnumType::VENDOR, GL_VENDOR>,
		EnumValuePair<EnumType::VERSION, GL_VERSION>,
		EnumValuePair<EnumType::MAX_VERTEX_ATTRIBS, GL_MAX_VERTEX_ATTRIBS>,
		EnumValuePair<EnumType::MAX_VERTEX_UNIFORM_COMPONENTS, GL_MAX_VERTEX_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_VERTEX_UNIFORM_VECTORS, GL_MAX_VERTEX_UNIFORM_VECTORS>,
		EnumValuePair<EnumType::MAX_VERTEX_UNIFORM_BLOCKS, GL_MAX_VERTEX_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_VERTEX_OUTPUT_COMPONENTS, GL_MAX_VERTEX_OUTPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_VERTEX_TEXTURE_IMAGE_UNITS, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_VERTEX_ATOMIC_COUNTER_BUFFERS, GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_VERTEX_ATOMIC_COUNTERS, GL_MAX_VERTEX_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_VERTEX_SHADER_STORAGE_BLOCKS, GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::MAX_TESS_GEN_LEVEL, GL_MAX_TESS_GEN_LEVEL>,
		EnumValuePair<EnumType::MAX_PATCH_VERTICES, GL_MAX_PATCH_VERTICES>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_UNIFORM_COMPONENTS, GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_OUTPUT_COMPONENTS, GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_PATCH_COMPONENTS, GL_MAX_TESS_PATCH_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_INPUT_COMPONENTS, GL_MAX_TESS_CONTROL_INPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_UNIFORM_BLOCKS, GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS, GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_ATOMIC_COUNTERS, GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_INPUT_COMPONENTS, GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_UNIFORM_BLOCKS, GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS, GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_ATOMIC_COUNTERS, GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_UNIFORM_COMPONENTS, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_UNIFORM_BLOCKS, GL_MAX_GEOMETRY_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_INPUT_COMPONENTS, GL_MAX_GEOMETRY_INPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_OUTPUT_COMPONENTS, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_OUTPUT_VERTICES, GL_MAX_GEOMETRY_OUTPUT_VERTICES>,
		EnumValuePair<EnumType::MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_SHADER_INVOCATIONS, GL_MAX_GEOMETRY_SHADER_INVOCATIONS>,
		EnumValuePair<EnumType::MAX_VERTEX_STREAMS, GL_MAX_VERTEX_STREAMS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS, GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_ATOMIC_COUNTERS, GL_MAX_GEOMETRY_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_UNIFORM_COMPONENTS, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_UNIFORM_VECTORS, GL_MAX_FRAGMENT_UNIFORM_VECTORS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_UNIFORM_BLOCKS, GL_MAX_FRAGMENT_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_INPUT_COMPONENTS, GL_MAX_FRAGMENT_INPUT_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TEXTURE_IMAGE_UNITS, GL_MAX_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MIN_PROGRAM_TEXTURE_GATHER_OFFSET, GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET>,
		EnumValuePair<EnumType::MAX_PROGRAM_TEXTURE_GATHER_OFFSET, GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET>,
		EnumValuePair<EnumType::MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS, GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_ATOMIC_COUNTERS, GL_MAX_FRAGMENT_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::MAX_COMPUTE_WORK_GROUP_COUNT, GL_MAX_COMPUTE_WORK_GROUP_COUNT>,
		EnumValuePair<EnumType::MAX_COMPUTE_WORK_GROUP_SIZE, GL_MAX_COMPUTE_WORK_GROUP_SIZE>,
		EnumValuePair<EnumType::MAX_COMPUTE_WORK_GROUP_INVOCATIONS, GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS>,
		EnumValuePair<EnumType::MAX_COMPUTE_UNIFORM_BLOCKS, GL_MAX_COMPUTE_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_COMPUTE_TEXTURE_IMAGE_UNITS, GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_COMPUTE_ATOMIC_COUNTERS, GL_MAX_COMPUTE_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_COMPUTE_SHARED_MEMORY_SIZE, GL_MAX_COMPUTE_SHARED_MEMORY_SIZE>,
		EnumValuePair<EnumType::MAX_COMPUTE_UNIFORM_COMPONENTS, GL_MAX_COMPUTE_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_COMPUTE_IMAGE_UNIFORMS, GL_MAX_COMPUTE_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_COMPUTE_SHADER_STORAGE_BLOCKS, GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::MIN_PROGRAM_TEXEL_OFFSET, GL_MIN_PROGRAM_TEXEL_OFFSET>,
		EnumValuePair<EnumType::MAX_PROGRAM_TEXEL_OFFSET, GL_MAX_PROGRAM_TEXEL_OFFSET>,
		EnumValuePair<EnumType::MAX_UNIFORM_BUFFER_BINDINGS, GL_MAX_UNIFORM_BUFFER_BINDINGS>,
		EnumValuePair<EnumType::MAX_UNIFORM_BLOCK_SIZE, GL_MAX_UNIFORM_BLOCK_SIZE>,
		EnumValuePair<EnumType::UNIFORM_BUFFER_OFFSET_ALIGNMENT, GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT>,
		EnumValuePair<EnumType::MAX_COMBINED_UNIFORM_BLOCKS, GL_MAX_COMBINED_UNIFORM_BLOCKS>,
		EnumValuePair<EnumType::MAX_VARYING_COMPONENTS, GL_MAX_VARYING_COMPONENTS>,
		EnumValuePair<EnumType::MAX_VARYING_VECTORS, GL_MAX_VARYING_VECTORS>,
		EnumValuePair<EnumType::MAX_VARYING_FLOATS, GL_MAX_VARYING_FLOATS>,
		EnumValuePair<EnumType::MAX_COMBINED_TEXTURE_IMAGE_UNITS, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_SUBROUTINES, GL_MAX_SUBROUTINES>,
		EnumValuePair<EnumType::MAX_SUBROUTINE_UNIFORM_LOCATIONS, GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS>,
		EnumValuePair<EnumType::MAX_UNIFORM_LOCATIONS, GL_MAX_UNIFORM_LOCATIONS>,
		EnumValuePair<EnumType::MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS>,
		EnumValuePair<EnumType::MAX_ATOMIC_COUNTER_BUFFER_SIZE, GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE>,
		EnumValuePair<EnumType::MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS>,
		EnumValuePair<EnumType::MAX_COMBINED_ATOMIC_COUNTERS, GL_MAX_COMBINED_ATOMIC_COUNTERS>,
		EnumValuePair<EnumType::MAX_SHADER_STORAGE_BUFFER_BINDINGS, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS>,
		EnumValuePair<EnumType::MAX_SHADER_STORAGE_BLOCK_SIZE, GL_MAX_SHADER_STORAGE_BLOCK_SIZE>,
		EnumValuePair<EnumType::MAX_COMBINED_SHADER_STORAGE_BLOCKS, GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS>,
		EnumValuePair<EnumType::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT>,
		EnumValuePair<EnumType::MAX_IMAGE_UNITS, GL_MAX_IMAGE_UNITS>,
		EnumValuePair<EnumType::MAX_COMBINED_SHADER_OUTPUT_RESOURCES, GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES>,
		EnumValuePair<EnumType::MAX_IMAGE_SAMPLES, GL_MAX_IMAGE_SAMPLES>,
		EnumValuePair<EnumType::MAX_VERTEX_IMAGE_UNIFORMS, GL_MAX_VERTEX_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_TESS_CONTROL_IMAGE_UNIFORMS, GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_TESS_EVALUATION_IMAGE_UNIFORMS, GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_GEOMETRY_IMAGE_UNIFORMS, GL_MAX_GEOMETRY_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_FRAGMENT_IMAGE_UNIFORMS, GL_MAX_FRAGMENT_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_COMBINED_IMAGE_UNIFORMS, GL_MAX_COMBINED_IMAGE_UNIFORMS>,
		EnumValuePair<EnumType::MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS, GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS, GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS>,
		EnumValuePair<EnumType::DEBUG_LOGGED_MESSAGES, GL_DEBUG_LOGGED_MESSAGES>,
		EnumValuePair<EnumType::DEBUG_NEXT_LOGGED_MESSAGE_LENGTH, GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH>,
		EnumValuePair<EnumType::DEBUG_OUTPUT_SYNCHRONOUS, GL_DEBUG_OUTPUT_SYNCHRONOUS>,
		EnumValuePair<EnumType::DEBUG_GROUP_STACK_DEPTH, GL_DEBUG_GROUP_STACK_DEPTH>,
		EnumValuePair<EnumType::DEBUG_OUTPUT, GL_DEBUG_OUTPUT>,
		EnumValuePair<EnumType::MAX_DEBUG_MESSAGE_LENGTH, GL_MAX_DEBUG_MESSAGE_LENGTH>,
		EnumValuePair<EnumType::MAX_DEBUG_LOGGED_MESSAGES, GL_MAX_DEBUG_LOGGED_MESSAGES>,
		EnumValuePair<EnumType::MAX_DEBUG_GROUP_STACK_DEPTH, GL_MAX_DEBUG_GROUP_STACK_DEPTH>,
		EnumValuePair<EnumType::MAX_LABEL_LENGTH, GL_MAX_LABEL_LENGTH>,
		EnumValuePair<EnumType::MAX_FRAMEBUFFER_WIDTH, GL_MAX_FRAMEBUFFER_WIDTH>,
		EnumValuePair<EnumType::MAX_FRAMEBUFFER_HEIGHT, GL_MAX_FRAMEBUFFER_HEIGHT>,
		EnumValuePair<EnumType::MAX_FRAMEBUFFER_LAYERS, GL_MAX_FRAMEBUFFER_LAYERS>,
		EnumValuePair<EnumType::MAX_FRAMEBUFFER_SAMPLES, GL_MAX_FRAMEBUFFER_SAMPLES>,
		EnumValuePair<EnumType::MAX_SAMPLE_MASK_WORDS, GL_MAX_SAMPLE_MASK_WORDS>,
		EnumValuePair<EnumType::MAX_SAMPLES, GL_MAX_SAMPLES>,
		EnumValuePair<EnumType::MAX_COLOR_TEXTURE_SAMPLES, GL_MAX_COLOR_TEXTURE_SAMPLES>,
		EnumValuePair<EnumType::MAX_DEPTH_TEXTURE_SAMPLES, GL_MAX_DEPTH_TEXTURE_SAMPLES>,
		EnumValuePair<EnumType::MAX_INTEGER_SAMPLES, GL_MAX_INTEGER_SAMPLES>,
		EnumValuePair<EnumType::MAX_SERVER_WAIT_TIMEOUT, GL_MAX_SERVER_WAIT_TIMEOUT>,
		EnumValuePair<EnumType::MIN_FRAGMENT_INTERPOLATION_OFFSET, GL_MIN_FRAGMENT_INTERPOLATION_OFFSET>,
		EnumValuePair<EnumType::MAX_FRAGMENT_INTERPOLATION_OFFSET, GL_MAX_FRAGMENT_INTERPOLATION_OFFSET>,
		EnumValuePair<EnumType::FRAGMENT_INTERPOLATION_OFFSET_BITS, GL_FRAGMENT_INTERPOLATION_OFFSET_BITS>,
		EnumValuePair<EnumType::MAX_DRAW_BUFFERS, GL_MAX_DRAW_BUFFERS>,
		EnumValuePair<EnumType::MAX_DUAL_SOURCE_DRAW_BUFFERS, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS>,
		EnumValuePair<EnumType::MAX_COLOR_ATTACHMENTS, GL_MAX_COLOR_ATTACHMENTS>,
		EnumValuePair<EnumType::MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS>,
		EnumValuePair<EnumType::MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS>,
		EnumValuePair<EnumType::MAX_TRANSFORM_FEEDBACK_BUFFERS, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS>,
		EnumValuePair<EnumType::DOUBLEBUFFER, GL_DOUBLEBUFFER>,
		EnumValuePair<EnumType::STEREO, GL_STEREO>,
		EnumValuePair<EnumType::SAMPLE_BUFFERS, GL_SAMPLE_BUFFERS>,
		EnumValuePair<EnumType::SAMPLES, GL_SAMPLES>,
		EnumValuePair<EnumType::IMPLEMENTATION_COLOR_READ_FORMAT, GL_IMPLEMENTATION_COLOR_READ_FORMAT>,
		EnumValuePair<EnumType::IMPLEMENTATION_COLOR_READ_TYPE, GL_IMPLEMENTATION_COLOR_READ_TYPE>,
		EnumValuePair<EnumType::QUERY_BUFFER_BINDING, GL_QUERY_BUFFER_BINDING>,
		EnumValuePair<EnumType::COPY_READ_BUFFER_BINDING, GL_COPY_READ_BUFFER_BINDING>,
		EnumValuePair<EnumType::COPY_WRITE_BUFFER_BINDING, GL_COPY_WRITE_BUFFER_BINDING>,
		EnumValuePair<EnumType::RESET_NOTIFICATION_STRATEGY, GL_RESET_NOTIFICATION_STRATEGY>,
		EnumValuePair<EnumType::TEXTURE_BUFFER_BINDING, GL_TEXTURE_BUFFER_BINDING>,
		EnumValuePair<EnumType::TEXTURE_CUBE_MAP_SEAMLESS, GL_TEXTURE_CUBE_MAP_SEAMLESS>,
		EnumValuePair<EnumType::TIMESTAMP, GL_TIMESTAMP>
	
	>;
};
