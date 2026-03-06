/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneShaderProgram.h>
#include <OvRendering/HAL/None/NoneTexture.h>

namespace
{
	std::unordered_map<std::string, OvRendering::Settings::UniformInfo> emptyUniforms;
}

template<>
OvRendering::HAL::NoneShaderProgram::TShaderProgram() 
{
}

template<>
OvRendering::HAL::NoneShaderProgram::~TShaderProgram()
{
}

template<>
void OvRendering::HAL::NoneShaderProgram::Bind() const
{
}

template<>
void OvRendering::HAL::NoneShaderProgram::Unbind() const
{
}

template<>
uint32_t OvRendering::HAL::NoneShaderProgram::GetID() const
{
	return 0;
}

template<>
void OvRendering::HAL::NoneShaderProgram::Attach(const NoneShaderStage& p_shader)
{
}

template<>
void OvRendering::HAL::NoneShaderProgram::Detach(const NoneShaderStage& p_shader)
{
	
}

template<>
void OvRendering::HAL::NoneShaderProgram::DetachAll()
{
}

template<>
OvRendering::Settings::ShaderLinkingResult OvRendering::HAL::NoneShaderProgram::Link()
{
	return {
		.success = true
	};
}

#define DECLARE_SET_UNIFORM_FUNCTION(type) \
template<> \
template<> \
void OvRendering::HAL::NoneShaderProgram::SetUniform<type>(const std::string&, const type&) \
{ \
}

#define DECLARE_GET_UNIFORM_FUNCTION(type) \
template<> \
template<> \
type OvRendering::HAL::NoneShaderProgram::GetUniform<type>(const std::string&) \
{ \
	return type{}; \
}

DECLARE_SET_UNIFORM_FUNCTION(int);
DECLARE_SET_UNIFORM_FUNCTION(float);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector2);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector3);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector4);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FMatrix4);

DECLARE_GET_UNIFORM_FUNCTION(int);
DECLARE_GET_UNIFORM_FUNCTION(float);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FVector2);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FVector3);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FVector4);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FMatrix4);

template<>
void OvRendering::HAL::NoneShaderProgram::QueryUniforms()
{
}

template<>
OvTools::Utils::OptRef<const OvRendering::Settings::UniformInfo> OvRendering::HAL::NoneShaderProgram::GetUniformInfo(const std::string& p_name) const
{
	return std::nullopt;
}

template<>
const std::unordered_map<std::string, OvRendering::Settings::UniformInfo>& OvRendering::HAL::NoneShaderProgram::GetUniforms() const
{
	return emptyUniforms;
}
