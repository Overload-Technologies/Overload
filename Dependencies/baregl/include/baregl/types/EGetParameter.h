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
	enum class EGetParameter : uint8_t
	{
		ACTIVE_TEXTURE,                                 // 1 int (GLenum)
		ALIASED_LINE_WIDTH_RANGE,                       // 2 floats
		ARRAY_BUFFER_BINDING,                           // 1 int
		BLEND,                                          // 1 boolean
		BLEND_COLOR,                                    // 4 floats
		BLEND_DST_ALPHA,                                // 1 int (GLenum)
		BLEND_DST_RGB,                                  // 1 int (GLenum)
		BLEND_EQUATION_RGB,                             // 1 int (GLenum)
		BLEND_EQUATION_ALPHA,                           // 1 int (GLenum)
		BLEND_SRC_ALPHA,                                // 1 int (GLenum)
		BLEND_SRC_RGB,                                  // 1 int (GLenum)
		COLOR_CLEAR_VALUE,                              // 4 floats
		COLOR_LOGIC_OP,                                 // 1 boolean
		COLOR_WRITEMASK,                                // 4 booleans
		COMPRESSED_TEXTURE_FORMATS,                     // n ints (GL_NUM_COMPRESSED_TEXTURE_FORMATS)
		MAX_COMPUTE_SHADER_STORAGE_BLOCKS,              // 1 int
		MAX_COMBINED_SHADER_STORAGE_BLOCKS,             // 1 int
		MAX_COMPUTE_UNIFORM_BLOCKS,                     // 1 int
		MAX_COMPUTE_TEXTURE_IMAGE_UNITS,                // 1 int
		MAX_COMPUTE_UNIFORM_COMPONENTS,                 // 1 int
		MAX_COMPUTE_ATOMIC_COUNTERS,                    // 1 int
		MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,             // 1 int
		MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,        // 1 int
		MAX_COMPUTE_WORK_GROUP_INVOCATIONS,             // 1 int
		MAX_COMPUTE_WORK_GROUP_COUNT,                   // 1 int (indexed)
		MAX_COMPUTE_WORK_GROUP_SIZE,                    // 1 int (indexed)
		DISPATCH_INDIRECT_BUFFER_BINDING,               // 1 int
		MAX_DEBUG_GROUP_STACK_DEPTH,                    // 1 int
		DEBUG_GROUP_STACK_DEPTH,                        // 1 int
		CONTEXT_FLAGS,                                  // 1 int
		CULL_FACE,                                      // 1 boolean
		CULL_FACE_MODE,                                 // 1 int (GLenum)
		CURRENT_PROGRAM,                                // 1 int
		DEPTH_CLEAR_VALUE,                              // 1 double/float
		DEPTH_FUNC,                                     // 1 int (GLenum)
		DEPTH_RANGE,                                    // 2 doubles/floats
		DEPTH_TEST,                                     // 1 boolean
		DEPTH_WRITEMASK,                                // 1 boolean
		DITHER,                                         // 1 boolean
		DOUBLEBUFFER,                                   // 1 boolean
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
		DRAW_FRAMEBUFFER_BINDING,                       // 1 int
		READ_FRAMEBUFFER_BINDING,                       // 1 int
		ELEMENT_ARRAY_BUFFER_BINDING,                   // 1 int
		FRAGMENT_SHADER_DERIVATIVE_HINT,                // 1 int (GLenum)
		IMPLEMENTATION_COLOR_READ_FORMAT,               // 1 int (GLenum)
		IMPLEMENTATION_COLOR_READ_TYPE,                 // 1 int (GLenum)
		LINE_SMOOTH,                                    // 1 boolean
		LINE_SMOOTH_HINT,                               // 1 int (GLenum)
		LINE_WIDTH,                                     // 1 float
		LAYER_PROVOKING_VERTEX,                         // 1 int (GLenum)
		LOGIC_OP_MODE,                                  // 1 int (GLenum)
		MAJOR_VERSION,                                  // 1 int
		MAX_3D_TEXTURE_SIZE,                            // 1 int
		MAX_ARRAY_TEXTURE_LAYERS,                       // 1 int
		MAX_CLIP_DISTANCES,                             // 1 int
		MAX_COLOR_TEXTURE_SAMPLES,                      // 1 int
		MAX_COMBINED_ATOMIC_COUNTERS,                   // 1 int
		MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,       // 1 int
		MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,       // 1 int
		MAX_COMBINED_TEXTURE_IMAGE_UNITS,               // 1 int
		MAX_COMBINED_UNIFORM_BLOCKS,                    // 1 int
		MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,         // 1 int
		MAX_CUBE_MAP_TEXTURE_SIZE,                      // 1 int
		MAX_DEPTH_TEXTURE_SAMPLES,                      // 1 int
		MAX_DRAW_BUFFERS,                               // 1 int
		MAX_DUAL_SOURCE_DRAW_BUFFERS,                   // 1 int
		MAX_ELEMENTS_INDICES,                           // 1 int
		MAX_ELEMENTS_VERTICES,                          // 1 int
		MAX_FRAGMENT_ATOMIC_COUNTERS,                   // 1 int
		MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,             // 1 int
		MAX_FRAGMENT_INPUT_COMPONENTS,                  // 1 int
		MAX_FRAGMENT_UNIFORM_COMPONENTS,                // 1 int
		MAX_FRAGMENT_UNIFORM_VECTORS,                   // 1 int
		MAX_FRAGMENT_UNIFORM_BLOCKS,                    // 1 int
		MAX_FRAMEBUFFER_WIDTH,                          // 1 int
		MAX_FRAMEBUFFER_HEIGHT,                         // 1 int
		MAX_FRAMEBUFFER_LAYERS,                         // 1 int
		MAX_FRAMEBUFFER_SAMPLES,                        // 1 int
		MAX_GEOMETRY_ATOMIC_COUNTERS,                   // 1 int
		MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,             // 1 int
		MAX_GEOMETRY_INPUT_COMPONENTS,                  // 1 int
		MAX_GEOMETRY_OUTPUT_COMPONENTS,                 // 1 int
		MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,               // 1 int
		MAX_GEOMETRY_UNIFORM_BLOCKS,                    // 1 int
		MAX_GEOMETRY_UNIFORM_COMPONENTS,                // 1 int
		MAX_INTEGER_SAMPLES,                            // 1 int
		MIN_MAP_BUFFER_ALIGNMENT,                       // 1 int
		MAX_LABEL_LENGTH,                               // 1 int
		MAX_PROGRAM_TEXEL_OFFSET,                       // 1 int
		MIN_PROGRAM_TEXEL_OFFSET,                       // 1 int
		MAX_RECTANGLE_TEXTURE_SIZE,                     // 1 int
		MAX_RENDERBUFFER_SIZE,                          // 1 int
		MAX_SAMPLE_MASK_WORDS,                          // 1 int
		MAX_SERVER_WAIT_TIMEOUT,                        // 1 int64
		MAX_SHADER_STORAGE_BUFFER_BINDINGS,             // 1 int
		MAX_TESS_CONTROL_ATOMIC_COUNTERS,               // 1 int
		MAX_TESS_EVALUATION_ATOMIC_COUNTERS,            // 1 int
		MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,         // 1 int
		MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,      // 1 int
		MAX_TEXTURE_BUFFER_SIZE,                        // 1 int
		MAX_TEXTURE_IMAGE_UNITS,                        // 1 int
		MAX_TEXTURE_LOD_BIAS,                           // 1 float
		MAX_TEXTURE_SIZE,                               // 1 int
		MAX_UNIFORM_BUFFER_BINDINGS,                    // 1 int
		MAX_UNIFORM_BLOCK_SIZE,                         // 1 int
		MAX_UNIFORM_LOCATIONS,                          // 1 int
		MAX_VARYING_COMPONENTS,                         // 1 int
		MAX_VARYING_VECTORS,                            // 1 int
		MAX_VARYING_FLOATS,                             // 1 int
		MAX_VERTEX_ATOMIC_COUNTERS,                     // 1 int
		MAX_VERTEX_ATTRIBS,                             // 1 int
		MAX_VERTEX_SHADER_STORAGE_BLOCKS,               // 1 int
		MAX_VERTEX_TEXTURE_IMAGE_UNITS,                 // 1 int
		MAX_VERTEX_UNIFORM_COMPONENTS,                  // 1 int
		MAX_VERTEX_UNIFORM_VECTORS,                     // 1 int
		MAX_VERTEX_OUTPUT_COMPONENTS,                   // 1 int
		MAX_VERTEX_UNIFORM_BLOCKS,                      // 1 int
		MAX_VIEWPORT_DIMS,                              // 2 ints
		MAX_VIEWPORTS,                                  // 1 int
		MINOR_VERSION,                                  // 1 int
		NUM_COMPRESSED_TEXTURE_FORMATS,                 // 1 int
		NUM_EXTENSIONS,                                 // 1 int
		NUM_PROGRAM_BINARY_FORMATS,                     // 1 int
		NUM_SHADER_BINARY_FORMATS,                      // 1 int
		PACK_ALIGNMENT,                                 // 1 int
		PACK_IMAGE_HEIGHT,                              // 1 int
		PACK_LSB_FIRST,                                 // 1 boolean
		PACK_ROW_LENGTH,                                // 1 int
		PACK_SKIP_IMAGES,                               // 1 int
		PACK_SKIP_PIXELS,                               // 1 int
		PACK_SKIP_ROWS,                                 // 1 int
		PACK_SWAP_BYTES,                                // 1 boolean
		PIXEL_PACK_BUFFER_BINDING,                      // 1 int
		PIXEL_UNPACK_BUFFER_BINDING,                    // 1 int
		POINT_FADE_THRESHOLD_SIZE,                      // 1 float
		PRIMITIVE_RESTART_INDEX,                        // 1 int
		PROGRAM_BINARY_FORMATS,                         // n ints (GL_NUM_PROGRAM_BINARY_FORMATS)
		PROGRAM_PIPELINE_BINDING,                       // 1 int
		PROGRAM_POINT_SIZE,                             // 1 boolean
		PROVOKING_VERTEX,                               // 1 int (GLenum)
		POINT_SIZE,                                     // 1 float
		POINT_SIZE_GRANULARITY,                         // 1 float
		POINT_SIZE_RANGE,                               // 2 floats
		POLYGON_OFFSET_FACTOR,                          // 1 float
		POLYGON_OFFSET_UNITS,                           // 1 float
		POLYGON_OFFSET_CLAMP,                           // 1 float
		POLYGON_OFFSET_FILL,                            // 1 boolean
		POLYGON_OFFSET_LINE,                            // 1 boolean
		POLYGON_OFFSET_POINT,                           // 1 boolean
		POLYGON_SMOOTH,                                 // 1 boolean
		POLYGON_SMOOTH_HINT,                            // 1 int (GLenum)
		POLYGON_MODE,                                   // 1 int (GLenum)
		READ_BUFFER,                                    // 1 int (GLenum)
		RENDERBUFFER_BINDING,                           // 1 int
		SAMPLE_BUFFERS,                                 // 1 int
		SAMPLE_COVERAGE_VALUE,                          // 1 float
		SAMPLE_COVERAGE_INVERT,                         // 1 boolean
		SAMPLE_MASK_VALUE,                              // 1 int
		SAMPLER_BINDING,                                // 1 int
		SAMPLES,                                        // 1 int
		SCISSOR_BOX,                                    // 4 ints
		SCISSOR_TEST,                                   // 1 boolean
		SHADER_COMPILER,                                // 1 boolean
		SHADER_STORAGE_BUFFER_BINDING,                  // 1 int (or indexed)
		SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,         // 1 int
		SHADER_STORAGE_BUFFER_START,                    // 1 int64 (indexed)
		SHADER_STORAGE_BUFFER_SIZE,                     // 1 int64 (indexed)
		SMOOTH_LINE_WIDTH_RANGE,                        // 2 floats
		SMOOTH_LINE_WIDTH_GRANULARITY,                  // 1 float
		STENCIL_BACK_FAIL,                              // 1 int (GLenum)
		STENCIL_BACK_FUNC,                              // 1 int (GLenum)
		STENCIL_BACK_PASS_DEPTH_FAIL,                   // 1 int (GLenum)
		STENCIL_BACK_PASS_DEPTH_PASS,                   // 1 int (GLenum)
		STENCIL_BACK_REF,                               // 1 int
		STENCIL_BACK_VALUE_MASK,                        // 1 int
		STENCIL_BACK_WRITEMASK,                         // 1 int
		STENCIL_CLEAR_VALUE,                            // 1 int
		STENCIL_FAIL,                                   // 1 int (GLenum)
		STENCIL_FUNC,                                   // 1 int (GLenum)
		STENCIL_PASS_DEPTH_FAIL,                        // 1 int (GLenum)
		STENCIL_PASS_DEPTH_PASS,                        // 1 int (GLenum)
		STENCIL_REF,                                    // 1 int
		STENCIL_TEST,                                   // 1 boolean
		STENCIL_VALUE_MASK,                             // 1 int
		STENCIL_WRITEMASK,                              // 1 int
		STEREO,                                         // 1 boolean
		SUBPIXEL_BITS,                                  // 1 int
		TEXTURE_BINDING_1D,                             // 1 int
		TEXTURE_BINDING_1D_ARRAY,                       // 1 int
		TEXTURE_BINDING_2D,                             // 1 int
		TEXTURE_BINDING_2D_ARRAY,                       // 1 int
		TEXTURE_BINDING_2D_MULTISAMPLE,                 // 1 int
		TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,           // 1 int
		TEXTURE_BINDING_3D,                             // 1 int
		TEXTURE_BINDING_BUFFER,                         // 1 int
		TEXTURE_BINDING_CUBE_MAP,                       // 1 int
		TEXTURE_BINDING_RECTANGLE,                      // 1 int
		TEXTURE_COMPRESSION_HINT,                       // 1 int (GLenum)
		TEXTURE_BUFFER_OFFSET_ALIGNMENT,                // 1 int
		TIMESTAMP,                                      // 1 int64
		TRANSFORM_FEEDBACK_BUFFER_BINDING,              // 1 int (or indexed)
		TRANSFORM_FEEDBACK_BUFFER_START,                // 1 int64 (indexed)
		TRANSFORM_FEEDBACK_BUFFER_SIZE,                 // 1 int64 (indexed)
		UNIFORM_BUFFER_BINDING,                         // 1 int (or indexed)
		UNIFORM_BUFFER_OFFSET_ALIGNMENT,                // 1 int
		UNIFORM_BUFFER_SIZE,                            // 1 int64 (indexed)
		UNIFORM_BUFFER_START,                           // 1 int64 (indexed)
		UNPACK_ALIGNMENT,                               // 1 int
		UNPACK_IMAGE_HEIGHT,                            // 1 int
		UNPACK_LSB_FIRST,                               // 1 boolean
		UNPACK_ROW_LENGTH,                              // 1 int
		UNPACK_SKIP_IMAGES,                             // 1 int
		UNPACK_SKIP_PIXELS,                             // 1 int
		UNPACK_SKIP_ROWS,                               // 1 int
		UNPACK_SWAP_BYTES,                              // 1 boolean
		VERTEX_ARRAY_BINDING,                           // 1 int
		VERTEX_BINDING_DIVISOR,                         // 1 int (indexed)
		VERTEX_BINDING_OFFSET,                          // 1 int (indexed)
		VERTEX_BINDING_STRIDE,                          // 1 int (indexed)
		VERTEX_BINDING_BUFFER,                          // 1 int (indexed)
		MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,              // 1 int
		MAX_VERTEX_ATTRIB_BINDINGS,                     // 1 int
		VIEWPORT,                                       // 4 ints (or indexed)
		VIEWPORT_BOUNDS_RANGE,                          // 2 ints
		VIEWPORT_INDEX_PROVOKING_VERTEX,                // 1 int (GLenum)
		VIEWPORT_SUBPIXEL_BITS,                         // 1 int
		MAX_ELEMENT_INDEX                               // 1 int64
	};
}
