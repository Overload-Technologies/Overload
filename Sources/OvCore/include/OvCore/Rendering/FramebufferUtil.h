/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

#include <OvRendering/HAL/Framebuffer.h>

namespace OvCore::Rendering::FramebufferUtil
{
	/**
	* Prepare the given framebuffer for rendering by attaching
	* the necessary textures and renderbuffers.
	* @note This overload gives more control over the texture desc.
	* @param p_framebuffer
	* @param p_textureDesc
	* @param p_useDepth
	* @param p_useStencil
	*/
	void SetupFramebuffer(
		OvRendering::HAL::Framebuffer& p_framebuffer,
		const OvRendering::Settings::TextureDesc& p_textureDesc,
		bool p_useDepth = true,
		bool p_useStencil = false
	);

	/**
	* Prepare the given framebuffer for rendering by attaching
	* the necessary textures and renderbuffers
	* @param p_framebuffer
	* @param p_width
	* @param p_height
	* @param p_useDepth
	* @param p_useStencil
	* @param p_useMipMaps
	*/
	void SetupFramebuffer(
		OvRendering::HAL::Framebuffer& p_framebuffer,
		uint32_t p_width = 0,
		uint32_t p_height = 0,
		bool p_useDepth = true,
		bool p_useStencil = false,
		bool p_useMipMaps = false
	);
}
