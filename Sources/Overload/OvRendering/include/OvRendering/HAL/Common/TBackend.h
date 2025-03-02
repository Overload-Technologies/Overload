/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Settings/DriverSettings.h>
#include <OvRendering/Settings/ERenderingCapability.h>
#include <OvRendering/Settings/EPrimitiveMode.h>
#include <OvRendering/Settings/ERasterizationMode.h>
#include <OvRendering/Settings/EComparaisonAlgorithm.h>
#include <OvRendering/Settings/EOperation.h>
#include <OvRendering/Settings/ECullFace.h>
#include <OvRendering/Settings/ECullingOptions.h>
#include <OvRendering/Settings/EPixelDataFormat.h>
#include <OvRendering/Settings/EPixelDataType.h>
#include <OvRendering/Settings/EGraphicsBackend.h>
#include <OvRendering/Data/PipelineState.h>

namespace OvRendering::HAL
{
	/**
	* Backend class that wraps the selected graphics API's context.
	*/
	template<Settings::EGraphicsBackend Backend, class Context>
	class TBackend final
	{
	public:
		/**
		* Initializes the backend.
		* @param debug Indicates whether to enable debug mode.
		* @return An optional pipeline state.
		*/
		std::optional<Data::PipelineState> Init(bool p_debug);

		/**
		* Clears the specified buffers.
		* @param p_colorBuffer Clears the color buffer if true.
		* @param p_depthBuffer Clears the depth buffer if true.
		* @param p_stencilBuffer Clears the stencil buffer if true.
		*/
		void Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer);

		/**
		* Reads pixels from the framebuffer.
		* @param p_x The x-coordinate of the lower-left corner.
		* @param p_y The y-coordinate of the lower-left corner.
		* @param p_width The width of the pixel rectangle.
		* @param p_height The height of the pixel rectangle.
		* @param p_format The format of the pixel data.
		* @param p_type The data type of the pixel data.
		* @param p_data The destination buffer to store pixel data.
		*/
		void ReadPixels(
			uint32_t p_x,
			uint32_t p_y,
			uint32_t p_width,
			uint32_t p_height,
			Settings::EPixelDataFormat p_format,
			Settings::EPixelDataType p_type,
			void* p_data
		);

		/**
		* Renders primitives from array data.
		* @param p_primitiveMode Specifies the kind of primitives to render.
		* @param p_indexCount The number of elements to render.
		*/
		void DrawElements(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount);

		/**
		* Renders multiple instances of a set of elements.
		* @param p_primitiveMode Specifies the kind of primitives to render.
		* @param p_indexCount The number of elements to render.
		* @param p_instances The number of instances to render.
		*/
		void DrawElementsInstanced(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount, uint32_t p_instances);

		/**
		* Renders primitives from array data without indexing.
		* @param p_primitiveMode Specifies the kind of primitives to render.
		* @param p_vertexCount The number of vertices to render.
		*/
		void DrawArrays(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount);

		/**
		* Renders multiple instances of a set of vertices.
		* @param p_primitiveMode Specifies the kind of primitives to render.
		* @param p_vertexCount The number of vertices to render.
		* @param p_instances The number of instances to render.
		*/
		void DrawArraysInstanced(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount, uint32_t p_instances);

		/**
		* Sets the clear color for the color buffer.
		* @param p_red The red component of the clear color.
		* @param p_green The green component of the clear color.
		* @param p_blue The blue component of the clear color.
		* @param p_alpha The alpha component of the clear color.
		*/
		void SetClearColor(float p_red, float p_green, float p_blue, float p_alpha);

		/**
		* Sets the width for rasterized lines.
		* @param p_width The width of rasterized lines.
		*/
		void SetRasterizationLinesWidth(float p_width);

		/**
		* Specifies the rasterization mode.
		* @param p_rasterizationMode The desired rasterization mode.
		*/
		void SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode);

		/**
		* Enables or disables a specific rendering capability.
		* @param p_capability The rendering capability to modify.
		* @param p_value True to enable, false to disable.
		*/
		void SetCapability(Settings::ERenderingCapability p_capability, bool p_value);

		/**
		* Retrieves the status of a rendering capability.
		* @param p_capability The rendering capability to query.
		* @return True if enabled, false otherwise.
		*/
		bool GetCapability(Settings::ERenderingCapability p_capability);

		/**
		* Sets the stencil test function and reference value.
		* @param p_algorithm The comparison function to use.
		* @param p_reference The reference value for the stencil test.
		* @param p_mask A mask that is ANDed with both the reference value and the stored stencil value.
		*/
		void SetStencilAlgorithm(OvRendering::Settings::EComparaisonAlgorithm p_algorithm, int32_t p_reference, uint32_t p_mask);

		/**
		* Sets the depth comparison function.
		* @param p_algorithm The depth comparison function to use.
		*/
		void SetDepthAlgorithm(OvRendering::Settings::EComparaisonAlgorithm p_algorithm);

		/**
		* Controls the writing of individual bits in the stencil planes.
		* @param p_mask Specifies a mask to enable and disable writing of individual bits in the stencil planes.
		*/
		void SetStencilMask(uint32_t p_mask);

		/**
		* Sets stencil test actions depending on test outcomes.
		* @param p_stencilFail Action to take when the stencil test fails.
		* @param p_depthFail Action to take when the stencil test passes, but the depth test fails.
		* @param p_bothPass Action to take when both the stencil test and depth test pass.
		*/
		void SetStencilOperations(
			Settings::EOperation p_stencilFail,
			Settings::EOperation p_depthFail,
			Settings::EOperation p_bothPass
		);

		/**
		* Specifies whether front- or back-facing facets can be culled.
		* @param p_cullFace Specifies the faces to cull.
		*/
		void SetCullFace(Settings::ECullFace p_cullFace);

		/**
		* Enables or disables writing into the depth buffer.
		* @param p_enable True to enable depth writing, false to disable.
		*/
		void SetDepthWriting(bool p_enable);

		/**
		* Enables or disables writing of frame buffer color components.
		* @param p_enableRed Enables writing of the red component.
		* @param p_enableGreen Enables writing of the green component.
		* @param p_enableBlue Enables writing of the blue component.
		* @param p_enableAlpha Enables writing of the alpha component.
		*/
		void SetColorWriting(bool p_enableRed, bool p_enableGreen, bool p_enableBlue, bool p_enableAlpha);

		/**
		* Sets the viewport transformation parameters.
		* @param p_x The lower left x-coordinate of the viewport rectangle.
		* @param p_y The lower left y-coordinate of the viewport rectangle.
		* @param p_width The width of the viewport.
		* @param p_height The height of the viewport.
		*/
		void SetViewport(uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height);

		/**
		* Retrieves the name of the graphics vendor.
		* @return A string containing the vendor name.
		*/
		std::string GetVendor();

		/**
		* Retrieves the name of the graphics hardware.
		* @return A string containing the hardware name.
		*/
		std::string GetHardware();

		/**
		* Retrieves the version of the graphics API in use.
		* @return A string containing the API version.
		*/
		std::string GetVersion();

		/**
		* Retrieves the version of the shading language.
		* @return A string containing the shading language version.
		*/
		std::string GetShadingLanguageVersion();

	private:
		Context m_context;
	};
}
