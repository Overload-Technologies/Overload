/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>

#include <baregl/Framebuffer.h>
#include <OvTools/Utils/CircularIterator.h>

namespace OvCore::Rendering
{
	/**
	* Convenient ping-pong buffer holding two framebuffers
	*/
	class PingPongFramebuffer : public OvTools::Utils::CircularIterator<baregl::Framebuffer, 2>
	{
	public:
		/**
		* Create a ping pong buffer
		* @param p_debugName
		*/
		PingPongFramebuffer(std::string_view p_debugName = std::string_view{});

		/**
		* Return the two framebuffers
		*/
		std::array<baregl::Framebuffer, 2>& GetFramebuffers();

	private:
		std::array<baregl::Framebuffer, 2> m_framebuffers;
	};
}
