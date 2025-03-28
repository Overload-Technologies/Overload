/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/Resources/Shader.h>

OvRendering::HAL::ShaderProgram& OvRendering::Resources::Shader::GetProgram() const
{
	OVASSERT(m_program != nullptr, "Trying to access a null ShaderProgram");
	return *m_program;
}

OvRendering::Resources::Shader::Shader(const std::string p_path, std::unique_ptr<HAL::ShaderProgram>&& p_program) : path(p_path)
{
	SetProgram(std::move(p_program));
}

void OvRendering::Resources::Shader::SetProgram(std::unique_ptr<HAL::ShaderProgram>&& p_program)
{
	OVASSERT(p_program != nullptr, "Cannot assign an invalid program!");
	m_program = std::move(p_program);
}
