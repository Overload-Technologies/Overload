/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace baregl::types
{
	/**
	* Parameter that can be use to retrieve associated values through GetX functions.
	*/
	enum class EGetParameter : uint16_t
	{
		// Current Values and Associated Data
		PATCH_VERTICES,                                 // 1 int
		PATCH_DEFAULT_OUTER_LEVEL,                      // 4 floats
		PATCH_DEFAULT_INNER_LEVEL,                      // 2 floats

		// Vertex Array Object State
		// VERTEX_ATTRIB_ARRAY_ENABLED,                    // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_SIZE,                       // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_STRIDE,                     // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_TYPE,                       // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_NORMALIZED,                 // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_INTEGER,                    // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_LONG,                       // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_DIVISOR,                    // GetVertexAttribiv
		// VERTEX_ATTRIB_ARRAY_POINTER,                    // GetVertexAttribiv
		// GetObjectLabel (string)
		ELEMENT_ARRAY_BUFFER_BINDING,                   // 1 int
		// VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,             // GetVertexAttribiv
		// VERTEX_ATTRIB_BINDING,                          // GetVertexAttribiv
		// VERTEX_ATTRIB_RELATIVE_OFFSET,                  // GetVertexAttribiv
		VERTEX_BINDING_OFFSET,                          // 1 int64 (indexed)
		VERTEX_BINDING_STRIDE,                          // 1 int (indexed)
		VERTEX_BINDING_DIVISOR,                         // 1 int (indexed)
		VERTEX_BINDING_BUFFER,                          // 1 int (indexed)

		// Vertex Array Data (not in Vertex Array objects)
		ARRAY_BUFFER_BINDING,                           // 1 int
		DRAW_INDIRECT_BUFFER_BINDING,                   // 1 int
		VERTEX_ARRAY_BINDING,                           // 1 int
		PRIMITIVE_RESTART,                              // 1 boolean
		PRIMITIVE_RESTART_FIXED_INDEX,                  // 1 boolean
		PRIMITIVE_RESTART_INDEX,                        // 1 int

		// Buffer Object State
		// GetBufferSubData(),
		// BUFFER_SIZE,                                    // n int64 (GetBufferParameteri64v)
		// BUFFER_USAGE,                                   // n int (GetBufferParameteriv)
		// BUFFER_ACCESS,                                  // n int (GetBufferParameteriv)
		// BUFFER_ACCESS_FLAGS,                            // n int (GetBufferParameteriv)
		// BUFFER_IMMUTABLE_STORAGE,                       // n bool (GetBufferParameteriv)
		// BUFFER_STORAGE_FLAGS,                           // n int (GetBufferParameteriv)
		// BUFFER_MAPPED,                                  // n bool (GetBufferParameteriv)
		// BUFFER_MAP_POINTER,                             // n pointer (GetBufferPointerv)
		// BUFFER_MAP_OFFSET,                              // n int64 (GetBufferParameteri64v)
		// BUFFER_MAP_LENGTH,                              // n int64 (GetBufferParameteri64v)
		// GetObjectLabel (string)

		// Transformation state
		VIEWPORT,                                       // 4 float (or indexed)
		DEPTH_RANGE,                                    // 2 doubles/floats (or indexed)
		CLIP_DISTANCE0,                                 // 1 boolean
		CLIP_DISTANCE1,                                 // 1 boolean
		CLIP_DISTANCE2,                                 // 1 boolean
		CLIP_DISTANCE3,                                 // 1 boolean
		CLIP_DISTANCE4,                                 // 1 boolean
		CLIP_DISTANCE5,                                 // 1 boolean
		CLIP_DISTANCE6,                                 // 1 boolean
		CLIP_DISTANCE7,                                 // 1 boolean
		CLIP_ORIGIN,                                    // 1 int (GLenum)
		CLIP_DEPTH_MODE,                                // 1 int (GLenum)
		DEPTH_CLAMP,                                    // 1 boolean
		TRANSFORM_FEEDBACK_BINDING,                     // 1 int

		// Coloring
		CLAMP_READ_COLOR,                               // 1 int (GLenum)
		PROVOKING_VERTEX,                               // 1 int (GLenum)

		// Rasterization
		RASTERIZER_DISCARD,                             // 1 boolean
		POINT_SIZE,                                     // 1 float
		POINT_FADE_THRESHOLD_SIZE,                      // 1 float
		POINT_SPRITE_COORD_ORIGIN,                      // 1 int (GLenum)
		LINE_WIDTH,                                     // 1 float
		LINE_SMOOTH,                                    // 1 boolean
		CULL_FACE,                                      // 1 boolean
		CULL_FACE_MODE,                                 // 1 int (GLenum)
		FRONT_FACE,                                     // 1 int (GLenum)
		POLYGON_SMOOTH,                                 // 1 boolean
		POLYGON_MODE,                                   // 1 int (GLenum)
		POLYGON_OFFSET_FACTOR,                          // 1 float
		POLYGON_OFFSET_UNITS,                           // 1 float
		POLYGON_OFFSET_POINT,                           // 1 boolean
		POLYGON_OFFSET_LINE,                            // 1 boolean
		POLYGON_OFFSET_FILL,                            // 1 boolean

		// Multisampling
		MULTISAMPLE,                                    // 1 boolean
		SAMPLE_ALPHA_TO_COVERAGE,                       // 1 boolean
		SAMPLE_ALPHA_TO_ONE,                            // 1 boolean
		SAMPLE_COVERAGE,                                // 1 boolean
		SAMPLE_COVERAGE_VALUE,                          // 1 float
		SAMPLE_COVERAGE_INVERT,                         // 1 boolean
		SAMPLE_SHADING,                                 // 1 boolean
		MIN_SAMPLE_SHADING_VALUE,                       // 1 float
		SAMPLE_MASK,                                    // 1 boolean
		SAMPLE_MASK_VALUE,                              // 1 int (indexed)

		// Textures (state per texture unit)
		TEXTURE_BINDING_1D,                             // 1 int
		TEXTURE_BINDING_2D,                             // 1 int
		TEXTURE_BINDING_3D,                             // 1 int
		TEXTURE_BINDING_1D_ARRAY,                       // 1 int
		TEXTURE_BINDING_2D_ARRAY,                       // 1 int
		TEXTURE_BINDING_CUBE_MAP_ARRAY,                 // 1 int
		TEXTURE_BINDING_RECTANGLE,                      // 1 int
		TEXTURE_BINDING_BUFFER,                         // 1 int
		TEXTURE_BINDING_CUBE_MAP,                       // 1 int
		TEXTURE_BINDING_2D_MULTISAMPLE,                 // 1 int
		TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,           // 1 int
		SAMPLER_BINDING,                                // 1 int
		// TEXTURE_1D,                                     // image data (GetTexImage)
		// TEXTURE_2D,                                     // image data (GetTexImage)
		// TEXTURE_3D,                                     // image data (GetTexImage)
		// TEXTURE_1D_ARRAY,                               // image data (GetTexImage)
		// TEXTURE_2D_ARRAY,                               // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_ARRAY,                         // image data (GetTexImage)
		// TEXTURE_RECTANGLE,                              // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_POSITIVE_X,                    // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_NEGATIVE_X,                    // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_POSITIVE_Y,                    // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_NEGATIVE_Y,                    // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_POSITIVE_Z,                    // image data (GetTexImage)
		// TEXTURE_CUBE_MAP_NEGATIVE_Z,                    // image data (GetTexImage)

		// Textures (state per texture object)
		// TEXTURE_SWIZZLE_R,                              // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_SWIZZLE_G,                              // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_SWIZZLE_B,                              // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_SWIZZLE_A,                              // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_BORDER_COLOR,                           // 4 floats/ints (GetTexParameter* / GetSamplerParameter*)
		// TEXTURE_MIN_FILTER,                             // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_MAG_FILTER,                             // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_WRAP_S,                                 // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_WRAP_T,                                 // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_WRAP_R,                                 // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_TARGET,                                 // 1 int (GetTextureParameteriv)
		// TEXTURE_MIN_LOD,                                // 1 float (GetTexParameterfv / GetSamplerParameterfv)
		// TEXTURE_MAX_LOD,                                // 1 float (GetTexParameterfv / GetSamplerParameterfv)
		// TEXTURE_BASE_LEVEL,                             // 1 int (GetTexParameteriv)
		// TEXTURE_MAX_LEVEL,                              // 1 int (GetTexParameteriv)
		// TEXTURE_LOD_BIAS,                               // 1 float (GetTexParameterfv / GetSamplerParameterfv)
		// DEPTH_STENCIL_TEXTURE_MODE,                     // 1 int (GetTexParameteriv)
		// TEXTURE_COMPARE_MODE,                           // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// TEXTURE_COMPARE_FUNC,                           // 1 int (GetTexParameteriv / GetSamplerParameteriv)
		// IMAGE_FORMAT_COMPATIBILITY_TYPE,                // 1 int (GetTexParameteriv)
		// TEXTURE_IMMUTABLE_FORMAT,                       // 1 bool (GetTexParameteriv)
		// TEXTURE_IMMUTABLE_LEVELS,                       // 1 int (GetTexParameteriv)
		// TEXTURE_VIEW_MIN_LEVEL,                         // 1 int (GetTexParameteriv)
		// TEXTURE_VIEW_NUM_LEVELS,                        // 1 int (GetTexParameteriv)
		// TEXTURE_VIEW_MIN_LAYER,                         // 1 int (GetTexParameteriv)
		// TEXTURE_VIEW_NUM_LAYERS,                        // 1 int (GetTexParameteriv)
		// _, (GetObjectLabel)

		// Textures (state per texture image)
		// TEXTURE_WIDTH,                                  // 1 int (GetTexLevelParameteriv)
		// TEXTURE_HEIGHT,                                 // 1 int (GetTexLevelParameteriv)
		// TEXTURE_DEPTH,                                  // 1 int (GetTexLevelParameteriv)
		// TEXTURE_SAMPLES,                                // 1 int (GetTexLevelParameteriv)
		// TEXTURE_FIXED_SAMPLE_LOCATIONS,                 // 1 bool (GetTexLevelParameteriv)
		// TEXTURE_INTERNAL_FORMAT,                        // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_RED_SIZE,                               // 1 int (GetTexLevelParameteriv)
		// TEXTURE_GREEN_SIZE,                             // 1 int (GetTexLevelParameteriv)
		// TEXTURE_BLUE_SIZE,                              // 1 int (GetTexLevelParameteriv)
		// TEXTURE_ALPHA_SIZE,                             // 1 int (GetTexLevelParameteriv)
		// TEXTURE_DEPTH_SIZE,                             // 1 int (GetTexLevelParameteriv)
		// TEXTURE_STENCIL_SIZE,                           // 1 int (GetTexLevelParameteriv)
		// TEXTURE_SHARED_SIZE,                            // 1 int (GetTexLevelParameteriv)
		// TEXTURE_RED_TYPE,                               // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_GREEN_TYPE,                             // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_BLUE_TYPE,                              // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_ALPHA_TYPE,                             // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_DEPTH_TYPE,                             // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_STENCIL_TYPE,                           // 1 GLenum (GetTexLevelParameteriv)
		// TEXTURE_COMPRESSED,                             // 1 bool (GetTexLevelParameteriv)
		// TEXTURE_COMPRESSED_IMAGE_SIZE,                  // 1 int (GetTexLevelParameteriv)
		// TEXTURE_BUFFER_DATA_STORE_BINDING,              // 1 int (GetTexLevelParameteriv)
		// TEXTURE_BUFFER_OFFSET,                          // 1 int (GetTexLevelParameteriv)
		// TEXTURE_BUFFER_SIZE,                            // 1 int (GetTexLevelParameteriv)
		
		// Textures (state per sampler object)
		// TEXTURE_BORDER_COLOR,                           // GetSamplerParameterfv
		// TEXTURE_COMPARE_FUNC,                           // GetSamplerParameterfv
		// TEXTURE_COMPARE_MODE,                           // GetSamplerParameterfv
		// TEXTURE_LOD_BIAS,                               // GetSamplerParameterfv
		// TEXTURE_MAX_LOD,                                // GetSamplerParameterfv
		// TEXTURE_MAG_FILTER,                             // GetSamplerParameterfv
		// TEXTURE_MIN_FILTER,                             // GetSamplerParameterfv
		// TEXTURE_MIN_LOD,                                // GetSamplerParameterfv
		// TEXTURE_WRAP_S,                                 // GetSamplerParameterfv
		// TEXTURE_WRAP_T,                                 // GetSamplerParameterfv
		// TEXTURE_WRAP_R,                                 // GetSamplerParameterfv
		// _,                                              // GetObjectLabel

		// Texture Environment and Generation
		ACTIVE_TEXTURE,                                 // 1 int (GLenum)

		// Pixel Operations
		SCISSOR_TEST,                                   // 1 boolean (indexed)
		SCISSOR_BOX,                                    // 4 ints (indexed)
		STENCIL_TEST,                                   // 1 boolean
		STENCIL_FUNC,                                   // 1 int (GLenum)
		STENCIL_VALUE_MASK,                             // 1 int
		STENCIL_REF,                                    // 1 int
		STENCIL_FAIL,                                   // 1 int (GLenum)
		STENCIL_PASS_DEPTH_FAIL,                        // 1 int (GLenum)
		STENCIL_PASS_DEPTH_PASS,                        // 1 int (GLenum)
		STENCIL_BACK_FUNC,                              // 1 int (GLenum)
		STENCIL_BACK_VALUE_MASK,                        // 1 int
		STENCIL_BACK_REF,                               // 1 int
		STENCIL_BACK_FAIL,                              // 1 int (GLenum)
		STENCIL_BACK_PASS_DEPTH_FAIL,                   // 1 int (GLenum)
		STENCIL_BACK_PASS_DEPTH_PASS,                   // 1 int (GLenum)
		DEPTH_TEST,                                     // 1 boolean
		DEPTH_FUNC,                                     // 1 int (GLenum)
		BLEND,                                          // 1 boolean (indexed)
		BLEND_SRC_RGB,                                  // 1 int (GLenum) (indexed)
		BLEND_SRC_ALPHA,                                // 1 int (GLenum) (indexed)
		BLEND_DST_RGB,                                  // 1 int (GLenum) (indexed)
		BLEND_DST_ALPHA,                                // 1 int (GLenum) (indexed)
		BLEND_EQUATION_RGB,                             // 1 int (GLenum) (indexed)
		BLEND_EQUATION_ALPHA,                           // 1 int (GLenum) (indexed)
		BLEND_COLOR,                                    // 4 floats
		FRAMEBUFFER_SRGB,                               // 1 boolean
		DITHER,                                         // 1 boolean
		COLOR_LOGIC_OP,                                 // 1 boolean
		LOGIC_OP_MODE,                                  // 1 int (GLenum)

		// Framebuffer Control
		COLOR_WRITEMASK,                                // 4 booleans (or indexed)
		DEPTH_WRITEMASK,                                // 1 boolean
		STENCIL_WRITEMASK,                              // 1 int
		STENCIL_BACK_WRITEMASK,                         // 1 int
		COLOR_CLEAR_VALUE,                              // 4 floats
		DEPTH_CLEAR_VALUE,                              // 1 double/float
		STENCIL_CLEAR_VALUE,                            // 1 int

		// Framebuffer (state per target binding point)
		DRAW_FRAMEBUFFER_BINDING,                       // 1 int
		READ_FRAMEBUFFER_BINDING,                       // 1 int

		// Framebuffer (state per framebuffer object)
		DRAW_BUFFER,                                    // 1 int (GLenum)
		DRAW_BUFFER0,                                   // 1 int (GLenum)
		DRAW_BUFFER1,                                   // 1 int (GLenum)
		DRAW_BUFFER2,                                   // 1 int (GLenum)
		DRAW_BUFFER3,                                   // 1 int (GLenum)
		DRAW_BUFFER4,                                   // 1 int (GLenum)
		DRAW_BUFFER5,                                   // 1 int (GLenum)
		DRAW_BUFFER6,                                   // 1 int (GLenum)
		DRAW_BUFFER7,                                   // 1 int (GLenum)
		DRAW_BUFFER8,                                   // 1 int (GLenum)
		DRAW_BUFFER9,                                   // 1 int (GLenum)
		DRAW_BUFFER10,                                  // 1 int (GLenum)
		DRAW_BUFFER11,                                  // 1 int (GLenum)
		DRAW_BUFFER12,                                  // 1 int (GLenum)
		DRAW_BUFFER13,                                  // 1 int (GLenum)
		DRAW_BUFFER14,                                  // 1 int (GLenum)
		DRAW_BUFFER15,                                  // 1 int (GLenum)
		READ_BUFFER,                                    // 1 int (GLenum)
		// FRAMEBUFFER_DEFAULT_WIDTH,                      // 1 int (GetFramebufferParameteriv)
		// FRAMEBUFFER_DEFAULT_HEIGHT,                     // 1 int (GetFramebufferParameteriv)
		// FRAMEBUFFER_DEFAULT_LAYERS,                     // 1 int (GetFramebufferParameteriv)
		// FRAMEBUFFER_DEFAULT_SAMPLES,                    // 1 int (GetFramebufferParameteriv)
		// FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS,     // 1 bool (GetFramebufferParameteriv)
		// _,                                              // GetObjectLabel

		// Framebuffer (state per framebuffer object)
		// FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,             // 1 int (GLenum) (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,             // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,           // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,   // 1 int (GLenum) (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER,           // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_LAYERED,                 // 1 boolean (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING,          // 1 int (GLenum) (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE,          // 1 int (GLenum) (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_RED_SIZE,                // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,              // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,               // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,              // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,              // 1 int (GetFramebufferAttachmentParameteriv)
		// FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,            // 1 int (GetFramebufferAttachmentParameteriv)

		// Renderbuffer (state per target and binding point)
		RENDERBUFFER_BINDING,                           // 1 int

		// Renderbuffer (state per renderbuffer object)
		// RENDERBUFFER_WIDTH,                             // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_HEIGHT,                            // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_INTERNAL_FORMAT,                   // 1 GLenum (GetRenderbufferParameteriv)
		// RENDERBUFFER_RED_SIZE,                          // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_GREEN_SIZE,                        // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_BLUE_SIZE,                         // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_ALPHA_SIZE,                        // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_DEPTH_SIZE,                        // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_STENCIL_SIZE,                      // 1 int (GetRenderbufferParameteriv)
		// RENDERBUFFER_SAMPLES,                           // 1 int (GetRenderbufferParameteriv)
		// _,                                              // GetObjectLabel

		// Pixels
		UNPACK_SWAP_BYTES,                              // 1 boolean
		UNPACK_LSB_FIRST,                               // 1 boolean
		UNPACK_IMAGE_HEIGHT,                            // 1 int
		UNPACK_SKIP_IMAGES,                             // 1 int
		UNPACK_ROW_LENGTH,                              // 1 int
		UNPACK_SKIP_ROWS,                               // 1 int
		UNPACK_SKIP_PIXELS,                             // 1 int
		UNPACK_ALIGNMENT,                               // 1 int
		UNPACK_COMPRESSED_BLOCK_WIDTH,                  // 1 int
		UNPACK_COMPRESSED_BLOCK_HEIGHT,                 // 1 int
		UNPACK_COMPRESSED_BLOCK_DEPTH,                  // 1 int
		UNPACK_COMPRESSED_BLOCK_SIZE,                   // 1 int
		PIXEL_UNPACK_BUFFER_BINDING,                    // 1 int
		PACK_SWAP_BYTES,                                // 1 boolean
		PACK_LSB_FIRST,                                 // 1 boolean
		PACK_IMAGE_HEIGHT,                              // 1 int
		PACK_SKIP_IMAGES,                               // 1 int
		PACK_ROW_LENGTH,                                // 1 int
		PACK_SKIP_ROWS,                                 // 1 int
		PACK_SKIP_PIXELS,                               // 1 int
		PACK_ALIGNMENT,                                 // 1 int
		PACK_COMPRESSED_BLOCK_WIDTH,                    // 1 int
		PACK_COMPRESSED_BLOCK_HEIGHT,                   // 1 int
		PACK_COMPRESSED_BLOCK_DEPTH,                    // 1 int
		PACK_COMPRESSED_BLOCK_SIZE,                     // 1 int
		PIXEL_PACK_BUFFER_BINDING,                      // 1 int

		// Shader Object State
		// SHADER_TYPE,                                    // 1 int (GetShaderiv)
		// DELETE_STATUS,                                  // 1 bool (GetShaderiv)
		// COMPILE_STATUS,                                 // 1 bool (GetShaderiv)
		// _,                                              // GetShaderInfoLog
		// INFO_LOG_LENGTH,                                // 1 int (GetShaderiv)
		// SHADER_SOURCE_LENGTH,                           // 1 int (GetShaderiv)
		// _,                                              // GetObjectLabel

		// Program Pipeline Object State
		// ACTIVE_PROGRAM,                                 // 1 int (GetProgramPipelineiv)
		// VERTEX_SHADER,                                  // 1 int (GetProgramPipelineiv)
		// GEOMETRY_SHADER,                                // 1 int (GetProgramPipelineiv)
		// FRAGMENT_SHADER,                                // 1 int (GetProgramPipelineiv)
		// COMPUTE_SHADER,                                 // 1 int (GetProgramPipelineiv)
		// TESS_CONTROL_SHADER,                            // 1 int (GetProgramPipelineiv)
		// TESS_EVALUATION_SHADER,                         // 1 int (GetProgramPipelineiv)
		// VALIDATE_STATUS,                                // 1 bool (GetProgramPipelineiv)
		// _,                                              // GetProgramPipelineInfoLog
		// INFO_LOG_LENGTH,                                // 1 int (GetProgramPipelineiv)
		// _,                                              // GetObjectLabel
		
		// Program Object State
		CURRENT_PROGRAM,                                // 1 int
		PROGRAM_PIPELINE_BINDING,                       // 1 int
		// PROGRAM_SEPARABLE,                              // 1 bool (GetProgramiv)
		// DELETE_STATUS,                                  // 1 bool (GetProgramiv)
		// LINK_STATUS,                                    // 1 bool (GetProgramiv)
		// VALIDATE_STATUS,                                // 1 bool (GetProgramiv)
		// ATTACHED_SHADERS,                               // 1 int (GetProgramiv)
		// _,                                              // GetAttachedShaders
		// _,                                              // GetProgramInfoLog
		// INFO_LOG_LENGTH,                                // 1 int (GetProgramiv)
		// PROGRAM_BINARY_LENGTH,                          // 1 int (GetProgramiv)
		// PROGRAM_BINARY_RETRIEVABLE_HINT,                // 1 bool (GetProgramiv)
		// _,                                              // GetProgramBinary
		// COMPUTE_WORK_GROUP_SIZE,                        // 3 ints (GetProgramiv)
		// _,                                              // GetObjectLabel
		// ACTIVE_UNIFORMS,                                // 1 int (GetProgramiv)
		// _,                                              // GetProgramiv
		// _,                                              // GetActiveUniform
		// _,                                              // GetActiveUniform
		// _,                                              // GetActiveUniform
		// ACTIVE_UNIFORM_MAX_LENGTH,                      // 1 int (GetProgramiv)
		// _,                                              // GetUniform
		// ACTIVE_ATTRIBUTES,                              // 1 int (GetProgramiv)
		// _,                                              // GetAttribLocation
		// _,                                              // GetActiveAttrib
		// _,                                              // GetActiveAttrib
		// _,                                              // GetActiveAttrib
		// ACTIVE_ATTRIBUTE_MAX_LENGTH,                    // 1 int (GetProgramiv)
		// GEOMETRY_VERTICES_OUT,                          // 1 int (GetProgramiv)
		// GEOMETRY_INPUT_TYPE,                            // 1 GLenum (GetProgramiv)
		// GEOMETRY_OUTPUT_TYPE,                           // 1 GLenum (GetProgramiv)
		// GEOMETRY_SHADER_INVOCATIONS,                    // 1 int (GetProgramiv)
		// TRANSFORM_FEEDBACK_BUFFER_MODE,                 // 1 GLenum (GetProgramiv)
		// TRANSFORM_FEEDBACK_VARYINS,                     // 1 int (GetProgramiv)
		// TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,          // 1 int (GetProgramiv)
		// _,                                              // 1 int (GetTransformFeedbackVarying)
		// _,                                              // 1 int (GetTransformFeedbackVarying)
		// _,                                              // 1 int (GetTransformFeedbackVarying)
		// ACTIVE_UNIFORM_BLOCKS,                          // 1 int (GetProgramiv)
		// ACTIVE_UNIFORM_BLOCKS_MAX_NAME_LENGTH,          // 1 int (GetProgramiv)
		// UNIFORM_TYPE,                                   // n int (GetActiveUniformsiv)
		// UNIFORM_SIZE,                                   // n int (GetActiveUniformsiv)
		// UNIFORM_NAME_LENGTH,                            // n int (GetActiveUniformsiv)
		// UNIFORM_BLOCK_INDEX,                            // n int (GetActiveUniformsiv)
		// UNIFORM_OFFSET,                                 // n int (GetActiveUniformsiv)
		// UNIFORM_ARRAY_STRIDE,                           // n int (GetActiveUniformsiv)
		// UNIFORM_MATRIX_STRIDE,                          // n int (GetActiveUniformsiv)
		// UNIFORM_IS_ROW_MAJOR,                           // n booleans (GetActiveUniformsiv)
		// UNIFORM_BLOCK_BINDING,                          // 1 int (GetActiveUniformsiv)
		// UNIFORM_BLOCK_DATA_SIZE,                        // 1 int (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_NAME_LENGTH,                      // 1 int (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_ACTIVE_UNIFORMS,                  // 1 int (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,           // n int (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,      // 1 boolean (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER,// 1 boolean (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER, // 1 boolean (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER,    // 1 boolean (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER,    // 1 boolean (GetActiveUniformBlockiv)
		// UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER,     // 1 boolean (GetActiveUniformBlockiv)
		// TESS_CONTROL_OUTPUT_VERTICES,                   // 1 int (GetProgramiv)
		// TESS_GEN_MODE,                                  // 1 GLenum (GetProgramiv)
		// TESS_GEN_SPACING,                               // 1 GLenum (GetProgramiv)
		// TESS_GEN_VERTEX_ORDER,                          // 1 GLenum (GetProgramiv)
		// TESS_GEN_POINT_MODE,                            // 1 bool (GetProgramiv)
		// ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,            // n int (GetProgramStageiv)
		// ACTIVE_SUBROUTINE_UNIFORMS,                     // n int (GetProgramStageiv)
		// ACTIVE_SUBROUTINES,                             // n int (GetProgramStageiv)
		// ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,           // n int (GetProgramStageiv)
		// ACTIVE_SUBROUTINE_MAX_LENGTH,                   // n int (GetProgramStageiv)
		// NUM_COMPATIBLE_SUBROUTINES,                     // n int (GetActiveSubroutineUniformiv)
		// COMPATIBLE_SUBROUTINES,                         // n int (GetActiveSubroutineUniformiv)
		// UNIFORM_SIZE,                                   // n int (GetActiveSubroutineUniformiv)
		// UNIFORM_NAME_LENGTH,                            // n int (GetActiveSubroutineUniformiv)
		// _,                                              // n int (GetActiveSubroutineUniformName)
		// _,                                              // n int (GetActiveSubroutineName)
		// _,                                              // n int (GetActiveSubroutineName)
		// ACTIVE_ATOMIC_COUNTER_BUFFERS,                  // 1 int (GetProgramiv)
		// ATOMIC_COUNTER_BUFFER_BINDING,                  // n int (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_DATA_SIZE,                // n int (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS,   // n int (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES, // n int (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER, // n bool (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER, // n bool (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER, // n bool (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER, // n bool (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER, // n bool (GetActiveAtomicCounterBufferiv)
		// ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER, // n bool (GetActiveAtomicCounterBufferiv)
		// UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,            // n int (GetActiveUniformsiv)

		// Program Interface State
		// ACTIVE_RESOURCES,                               // n int (GetProgramInterfaceiv)
		// MAX_NAME_LENGTH,                                // n int (GetProgramInterfaceiv)
		// MAX_NUM_ACTIVE_VARIABLES,                       // n int (GetProgramInterfaceiv)
		// MAX_NUM_COMPATIBLE_SUBROUTINES,                 // n int (GetProgramInterfaceiv)

		// Program Object Resource State
		// ACTIVE_VARIABLES,                               // 1 int (GetProgramResourceiv)
		// ARRAY_SIZE,                                     // 1 int (GetProgramResourceiv)
		// ARRAY_STRIDE,                                   // 1 int (GetProgramResourceiv)
		// ATOMIC_COUNTER_BUFFER_INDEX,                    // 1 int (GetProgramResourceiv)
		// BLOCK_INDEX,                                    // 1 int (GetProgramResourceiv)
		// BUFFER_BINDING,                                 // 1 int (GetProgramResourceiv)
		// BUFFER_DATA_SIZE,                               // 1 int (GetProgramResourceiv)
		// COMPATIBLE_SUBROUTINES,                         // 1 int (GetProgramResourceiv)
		// IS_PER_PATCH,                                   // 1 int (GetProgramResourceiv)
		// IS_ROW_MAJOR,                                   // 1 int (GetProgramResourceiv)
		// LOCATION,                                       // 1 int (GetProgramResourceiv)
		// LOCATION_COMPONENT,                             // 1 int (GetProgramResourceiv)
		// LOCATION_INDEX,                                 // 1 int (GetProgramResourceiv)
		// MATRIX_STRIDE,                                  // 1 int (GetProgramResourceiv)
		// NAME_LENGTH,                                    // 1 int (GetProgramResourceiv)
		// NUM_ACTIVE_VARIABLES,                           // 1 int (GetProgramResourceiv)
		// NUM_COMPATIBLE_SUBROUTINES,                     // 1 int (GetProgramResourceiv)
		// OFFSET,                                         // 1 int (GetProgramResourceiv)
		// REFERENCED_BY_VERTEX_SHADER,                    // 1 int (GetProgramResourceiv)
		// REFERENCED_BY_TESS_CONTROL_SHADER,              // 1 int (GetProgramResourceiv)
		// REFERENCED_BY_TESS_EVALUATION_SHADER,           // 1 int (GetProgramResourceiv)
		// REFERENCED_BY_GEOMETRY_SHADER,                  // 1 int (GetProgramResourceiv)
		// REFERENCED_BY_FRAGMENT_SHADER,                  // 1 int (GetProgramResourceiv)
		// REFERENCED_BY_COMPUTE_SHADER,                   // 1 int (GetProgramResourceiv)
		// TOP_LEVEL_ARRAY_SIZE,                           // 1 int (GetProgramResourceiv)
		// TOP_LEVEL_ARRAY_STRIDE,                         // 1 int (GetProgramResourceiv)
		// TYPE,                                           // 1 int (GetProgramResourceiv)


		// Vertex and Geometry Shader State (not part of program objects)
		// CURRENT_VERTEX_ATTRIB,                          // ? floats (GetVertexAttribfv)
		PROGRAM_POINT_SIZE,                             // 1 boolean

		// Query Object State
		// QUERY_RESULT,                                   // 1 int (GetQueryObjectuiv)
		// QUERY_RESULT_AVAILABLE,                         // 1 int (GetQueryObjectiv)
		// GetObjectLabel,
		// QUERY_TARGET,                                   // 1 int (GLenum) (GetQueryObjectiv)

		// Image State (state per image unit)
		IMAGE_BINDING_NAME,                             // 1 int (indexed)
		IMAGE_BINDING_LEVEL,                            // 1 int (indexed)
		IMAGE_BINDING_LAYERED,                          // 1 boolean (indexed)
		IMAGE_BINDING_LAYER,                            // 1 int (indexed)
		IMAGE_BINDING_ACCESS,                           // 1 int (GLenum)(indexed)
		IMAGE_BINDING_FORMAT,                           // 1 int (GLenum)(indexed)

		// Atomic Counter Buffer Binding State
		ATOMIC_COUNTER_BUFFER_BINDING,                  // 1 int (or indexed)
		ATOMIC_COUNTER_BUFFER_START,                    // 1 int64 (indexed)
		ATOMIC_COUNTER_BUFFER_SIZE,                     // 1 int64 (indexed)

		// Shader Storage Buffer Binding State
		SHADER_STORAGE_BUFFER_BINDING,                  // 1 int (or indexed)
		SHADER_STORAGE_BUFFER_START,                    // 1 int64 (indexed)
		SHADER_STORAGE_BUFFER_SIZE,                     // 1 int64 (indexed)

		// Transform Feedback State
		TRANSFORM_FEEDBACK_BUFFER_BINDING,              // 1 int (or indexed)
		TRANSFORM_FEEDBACK_BUFFER_START,                // 1 int64 (indexed)
		TRANSFORM_FEEDBACK_BUFFER_SIZE,                 // 1 int64 (indexed)
		TRANSFORM_FEEDBACK_PAUSED,                      // 1 boolean
		TRANSFORM_FEEDBACK_ACTIVE,                      // 1 boolean
		// GetObjectLabel,

		// Uniform Buffer Binding State
		UNIFORM_BUFFER_BINDING,                         // 1 int (or indexed)
		UNIFORM_BUFFER_START,                           // 1 int64 (indexed)
		UNIFORM_BUFFER_SIZE,                            // 1 int64 (indexed)

		// Sync Object State
		// OBJECT_TYPE,                                    // 1 int (GLenum) (GetSynciv)
		// SYNC_STATUS,                                    // 1 int (GLenum) (GetSynciv)
		// SYNC_CONDITION,                                 // 1 int (GLenum) (GetSynciv)
		// SYNC_FLAGS,                                     // 1 int (GetSynciv)
		// GetObjectPtrLabel,

		// Hints
		LINE_SMOOTH_HINT,                               // 1 int (GLenum)
		POLYGON_SMOOTH_HINT,                            // 1 int (GLenum)
		TEXTURE_COMPRESSION_HINT,                       // 1 int (GLenum)
		FRAGMENT_SHADER_DERIVATIVE_HINT,                // 1 int (GLenum)

		// Compute Dispatch State
		DISPATCH_INDIRECT_BUFFER_BINDING,               // 1 int

		// Implementation Dependent Values
		CONTEXT_RELEASE_BEHAVIOR,                       // 1 GLenum
		MAX_CLIP_DISTANCES,                             // 1 int
		MAX_CULL_DISTANCES,                             // 1 int
		MAX_COMBINED_CLIP_AND_CULL_DISTANCES,           // 1 int
		SUBPIXEL_BITS,                                  // 1 int
		MAX_ELEMENT_INDEX,                              // 1 int64
		PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED,        // 1 bool
		MAX_3D_TEXTURE_SIZE,                            // 1 int
		MAX_TEXTURE_SIZE,                               // 1 int
		MAX_ARRAY_TEXTURE_LAYERS,                       // 1 int
		MAX_TEXTURE_LOD_BIAS,                           // 1 float
		MAX_CUBE_MAP_TEXTURE_SIZE,                      // 1 int
		MAX_RENDERBUFFER_SIZE,                          // 1 int
		MAX_VIEWPORT_DIMS,                              // 2 ints
		MAX_VIEWPORTS,                                  // 1 int
		VIEWPORT_SUBPIXEL_BITS,                         // 1 int
		VIEWPORT_BOUNDS_RANGE,                          // 2 ints
		LAYER_PROVOKING_VERTEX,                         // 1 int (GLenum)
		VIEWPORT_INDEX_PROVOKING_VERTEX,                // 1 int (GLenum)
		POINT_SIZE_RANGE,                               // 2 floats
		POINT_SIZE_GRANULARITY,                         // 1 float
		ALIASED_LINE_WIDTH_RANGE,                       // 2 floats
		SMOOTH_LINE_WIDTH_RANGE,                        // 2 floats
		SMOOTH_LINE_WIDTH_GRANULARITY,                  // 1 float
		MAX_ELEMENTS_INDICES,                           // 1 int
		MAX_ELEMENTS_VERTICES,                          // 1 int
		MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,              // 1 int
		MAX_VERTEX_ATTRIB_BINDINGS,                     // 1 int
		MAX_VERTEX_ATTRIB_STRIDE,                       // 1 int
		NUM_COMPRESSED_TEXTURE_FORMATS,                 // 1 int
		COMPRESSED_TEXTURE_FORMATS,                     // n ints (GL_NUM_COMPRESSED_TEXTURE_FORMATS)
		MAX_TEXTURE_BUFFER_SIZE,                        // 1 int
		MAX_RECTANGLE_TEXTURE_SIZE,                     // 1 int
		NUM_PROGRAM_BINARY_FORMATS,                     // 1 int
		PROGRAM_BINARY_FORMATS,                         // n ints (GL_NUM_PROGRAM_BINARY_FORMATS)
		NUM_SHADER_BINARY_FORMATS,                      // 1 int
		SHADER_BINARY_FORMATS,                          // n ints (GL_NUM_SHADER_BINARY_FORMATS)
		SHADER_COMPILER,                                // 1 boolean
		MIN_MAP_BUFFER_ALIGNMENT,                       // 1 int
		TEXTURE_BUFFER_OFFSET_ALIGNMENT,                // 1 int
		
		// Implementation Dependent Version and Extension Support
		MAJOR_VERSION,                                  // 1 int
		MINOR_VERSION,                                  // 1 int
		CONTEXT_FLAGS,                                  // 1 int
		CONTEXT_PROFILE_MASK,                           // 1 int
		EXTENSIONS,                                     // 1 int (indexed)
		NUM_EXTENSIONS,                                 // 1 int
		RENDERER,                                       // 1 string
		SHADING_LANGUAGE_VERSION,                       // 1 string (indexed)
		NUM_SHADING_LANGUAGE_VERSIONS,                  // 1 int
		VENDOR,                                         // 1 string
		VERSION,                                        // 1 string

		// Implementation Dependent Vertex Shader Limits
		MAX_VERTEX_ATTRIBS,                             // 1 int
		MAX_VERTEX_UNIFORM_COMPONENTS,                  // 1 int
		MAX_VERTEX_UNIFORM_VECTORS,                     // 1 int
		MAX_VERTEX_UNIFORM_BLOCKS,                      // 1 int
		MAX_VERTEX_OUTPUT_COMPONENTS,                   // 1 int
		MAX_VERTEX_TEXTURE_IMAGE_UNITS,                 // 1 int
		MAX_VERTEX_ATOMIC_COUNTER_BUFFERS,              // 1 int
		MAX_VERTEX_ATOMIC_COUNTERS,                     // 1 int
		MAX_VERTEX_SHADER_STORAGE_BLOCKS,               // 1 int
		
		// Implementation Dependent Tessellation Shader Limits
		MAX_TESS_GEN_LEVEL,                             // 1 int
		MAX_PATCH_VERTICES,                             // 1 int
		MAX_TESS_CONTROL_UNIFORM_COMPONENTS,            // 1 int
		MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS,           // 1 int
		MAX_TESS_CONTROL_OUTPUT_COMPONENTS,             // 1 int
		MAX_TESS_PATCH_COMPONENTS,                      // 1 int
		MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS,       // 1 int
		MAX_TESS_CONTROL_INPUT_COMPONENTS,              // 1 int
		MAX_TESS_CONTROL_UNIFORM_BLOCKS,                // 1 int
		MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS,        // 1 int
		MAX_TESS_CONTROL_ATOMIC_COUNTERS,               // 1 int
		MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,         // 1 int
		MAX_TESS_EVALUATION_UNIFORM_COMPONENTS,         // 1 int
		MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS,        // 1 int
		MAX_TESS_EVALUATION_OUTPUT_COMPONENTS,          // 1 int
		MAX_TESS_EVALUATION_INPUT_COMPONENTS,           // 1 int
		MAX_TESS_EVALUATION_UNIFORM_BLOCKS,             // 1 int
		MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS,     // 1 int
		MAX_TESS_EVALUATION_ATOMIC_COUNTERS,            // 1 int
		MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,      // 1 int

		// Implementation Dependent Geometry Shader Limits
		MAX_GEOMETRY_UNIFORM_COMPONENTS,                // 1 int
		MAX_GEOMETRY_UNIFORM_BLOCKS,                    // 1 int
		MAX_GEOMETRY_INPUT_COMPONENTS,                  // 1 int
		MAX_GEOMETRY_OUTPUT_COMPONENTS,                 // 1 int
		MAX_GEOMETRY_OUTPUT_VERTICES,                   // 1 int
		MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS,           // 1 int
		MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,               // 1 int
		MAX_GEOMETRY_SHADER_INVOCATIONS,                // 1 int
		MAX_VERTEX_STREAMS,                             // 1 int
		MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS,            // 1 int
		MAX_GEOMETRY_ATOMIC_COUNTERS,                   // 1 int
		MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,             // 1 int

		// Implementation Dependent Fragment Shader Limits
		MAX_FRAGMENT_UNIFORM_COMPONENTS,                // 1 int
		MAX_FRAGMENT_UNIFORM_VECTORS,                   // 1 int
		MAX_FRAGMENT_UNIFORM_BLOCKS,                    // 1 int
		MAX_FRAGMENT_INPUT_COMPONENTS,                  // 1 int
		MAX_TEXTURE_IMAGE_UNITS,                        // 1 int
		MIN_PROGRAM_TEXTURE_GATHER_OFFSET,              // 1 int
		MAX_PROGRAM_TEXTURE_GATHER_OFFSET,              // 1 int
		MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS,            // 1 int
		MAX_FRAGMENT_ATOMIC_COUNTERS,                   // 1 int
		MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,             // 1 int
		
		// Implementation Dependent Compute Shader Limits
		MAX_COMPUTE_WORK_GROUP_COUNT,                   // 3 int (indexed)
		MAX_COMPUTE_WORK_GROUP_SIZE,                    // 3 int (indexed)
		MAX_COMPUTE_WORK_GROUP_INVOCATIONS,             // 1 int
		MAX_COMPUTE_UNIFORM_BLOCKS,                     // 1 int
		MAX_COMPUTE_TEXTURE_IMAGE_UNITS,                // 1 int
		MAX_COMPUTE_ATOMIC_COUNTERS,                    // 1 int
		MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,             // 1 int
		MAX_COMPUTE_SHARED_MEMORY_SIZE,                 // 1 int
		MAX_COMPUTE_UNIFORM_COMPONENTS,                 // 1 int
		MAX_COMPUTE_IMAGE_UNIFORMS,                     // 1 int
		MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,        // 1 int
		MAX_COMPUTE_SHADER_STORAGE_BLOCKS,              // 1 int

		// Implementation Dependent Aggregate Shader Limits
		MIN_PROGRAM_TEXEL_OFFSET,                       // 1 int
		MAX_PROGRAM_TEXEL_OFFSET,                       // 1 int
		MAX_UNIFORM_BUFFER_BINDINGS,                    // 1 int
		MAX_UNIFORM_BLOCK_SIZE,                         // 1 int
		UNIFORM_BUFFER_OFFSET_ALIGNMENT,                // 1 int
		MAX_COMBINED_UNIFORM_BLOCKS,                    // 1 int
		MAX_VARYING_COMPONENTS,                         // 1 int
		MAX_VARYING_VECTORS,                            // 1 int
		MAX_VARYING_FLOATS,                             // 1 int
		MAX_COMBINED_TEXTURE_IMAGE_UNITS,               // 1 int
		MAX_SUBROUTINES,                                // 1 int
		MAX_SUBROUTINE_UNIFORM_LOCATIONS,               // 1 int
		MAX_UNIFORM_LOCATIONS,                          // 1 int
		MAX_ATOMIC_COUNTER_BUFFER_BINDINGS,             // 1 int
		MAX_ATOMIC_COUNTER_BUFFER_SIZE,                 // 1 int
		MAX_COMBINED_ATOMIC_COUNTER_BUFFERS,            // 1 int
		MAX_COMBINED_ATOMIC_COUNTERS,                   // 1 int
		MAX_SHADER_STORAGE_BUFFER_BINDINGS,             // 1 int
		MAX_SHADER_STORAGE_BLOCK_SIZE,                  // 1 int64
		MAX_COMBINED_SHADER_STORAGE_BLOCKS,             // 1 int
		SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,         // 1 int
		MAX_IMAGE_UNITS,                                // 1 int
		MAX_COMBINED_SHADER_OUTPUT_RESOURCES,           // 1 int
		MAX_IMAGE_SAMPLES,                              // 1 int
		MAX_VERTEX_IMAGE_UNIFORMS,                      // 1 int
		MAX_TESS_CONTROL_IMAGE_UNIFORMS,                // 1 int
		MAX_TESS_EVALUATION_IMAGE_UNIFORMS,             // 1 int
		MAX_GEOMETRY_IMAGE_UNIFORMS,                    // 1 int
		MAX_FRAGMENT_IMAGE_UNIFORMS,                    // 1 int
		MAX_COMBINED_IMAGE_UNIFORMS,                    // 1 int
		MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,         // 1 int
		MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,       // 1 int
		MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS,   // 1 int
		MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS,// 1 int
		MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,       // 1 int

		// Debug Output State
		// DEBUG_CALLBACK_FUNCTION,                        // 1 pointer (GetPointerv)
		// DEBUG_CALLBACK_USER_PARAM,                      // 1 pointer (GetPointerv)
		DEBUG_LOGGED_MESSAGES,                          // 1 int
		DEBUG_NEXT_LOGGED_MESSAGE_LENGTH,               // 1 int
		DEBUG_OUTPUT_SYNCHRONOUS,                       // 1 boolean
		DEBUG_GROUP_STACK_DEPTH,                        // 1 int
		DEBUG_OUTPUT,                                   // 1 boolean

		// Implementation Dependent Debug Output State
		MAX_DEBUG_MESSAGE_LENGTH,                       // 1 int
		MAX_DEBUG_LOGGED_MESSAGES,                      // 1 int
		MAX_DEBUG_GROUP_STACK_DEPTH,                    // 1 int
		MAX_LABEL_LENGTH,                               // 1 int

		// Implementation Dependent Values (cont.)
		MAX_FRAMEBUFFER_WIDTH,                          // 1 int
		MAX_FRAMEBUFFER_HEIGHT,                         // 1 int
		MAX_FRAMEBUFFER_LAYERS,                         // 1 int
		MAX_FRAMEBUFFER_SAMPLES,                        // 1 int
		MAX_SAMPLE_MASK_WORDS,                          // 1 int
		MAX_SAMPLES,                                    // 1 int
		MAX_COLOR_TEXTURE_SAMPLES,                      // 1 int
		MAX_DEPTH_TEXTURE_SAMPLES,                      // 1 int
		MAX_INTEGER_SAMPLES,                            // 1 int
		// QUERY_COUNTER_BITS,                             // 5 int (GetQueryiv)
		MAX_SERVER_WAIT_TIMEOUT,                        // 1 int64
		MIN_FRAGMENT_INTERPOLATION_OFFSET,              // 1 float
		MAX_FRAGMENT_INTERPOLATION_OFFSET,              // 1 float
		FRAGMENT_INTERPOLATION_OFFSET_BITS,             // 1 int
		MAX_DRAW_BUFFERS,                               // 1 int
		MAX_DUAL_SOURCE_DRAW_BUFFERS,                   // 1 int
		MAX_COLOR_ATTACHMENTS,                          // 1 int

		// Internal Format Dependent Values
		// SAMPLES,                                        // 1 int (GetInternalformativ)
		// NUM_SAMPLE_COUNTS,                              // 1 int (GetInternalformativ)

		// Implementation Dependent Transform Feedback Limits
		MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS,  // 1 int
		MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,        // 1 int
		MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS,     // 1 int
		MAX_TRANSFORM_FEEDBACK_BUFFERS,                 // 1 int

		// Framebuffer Dependent Values
		DOUBLEBUFFER,                                   // 1 boolean
		STEREO,                                         // 1 boolean
		SAMPLE_BUFFERS,                                 // 1 int
		SAMPLES,                                        // 1 int
		// SAMPLE_POSITION,                                // ??? (GetMultisamplefv)
		IMPLEMENTATION_COLOR_READ_FORMAT,               // 1 int (GLenum)
		IMPLEMENTATION_COLOR_READ_TYPE,                 // 1 int (GLenum)

		// Miscellaneous
		// CURRENT_QUERY,                                  // 1 int (GetQueryiv)
		QUERY_BUFFER_BINDING,                           // 1 int
		COPY_READ_BUFFER_BINDING,                       // 1 int
		COPY_WRITE_BUFFER_BINDING,                      // 1 int
		RESET_NOTIFICATION_STRATEGY,                    // 1 int
		TEXTURE_BUFFER_BINDING,                         // 1 int
		TEXTURE_CUBE_MAP_SEAMLESS,                      // 1 boolean
		TIMESTAMP,                                      // 1 int64
	};
}
