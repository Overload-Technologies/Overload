/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>

#include <OvCore/Rendering/PingPongFramebuffer.h>

OvCore::Rendering::PingPongFramebuffer::PingPongFramebuffer(std::string_view p_debugName) :
	CircularIterator<OvRendering::HAL::Framebuffer, 2>(m_framebuffers),
	m_framebuffers{
		OvRendering::HAL::Framebuffer{std::format("{}PingPong{}", p_debugName, 0)},
		OvRendering::HAL::Framebuffer{std::format("{}PingPong{}", p_debugName, 1)}
	}
{

}

std::array<OvRendering::HAL::Framebuffer, 2>& OvCore::Rendering::PingPongFramebuffer::GetFramebuffers()
{
	return m_framebuffers;
}
