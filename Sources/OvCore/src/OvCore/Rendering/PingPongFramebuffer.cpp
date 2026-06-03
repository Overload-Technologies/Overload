/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>

#include <OvCore/Rendering/PingPongFramebuffer.h>

OvCore::Rendering::PingPongFramebuffer::PingPongFramebuffer(std::string_view p_debugName) :
	CircularIterator<baregl::Framebuffer, 2>(m_framebuffers),
	m_framebuffers{
		baregl::Framebuffer{std::format("{}PingPong{}", p_debugName, 0)},
		baregl::Framebuffer{std::format("{}PingPong{}", p_debugName, 1)}
	}
{

}

std::array<baregl::Framebuffer, 2>& OvCore::Rendering::PingPongFramebuffer::GetFramebuffers()
{
	return m_framebuffers;
}
