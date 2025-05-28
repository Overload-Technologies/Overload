/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneTextureHandle.h>

template<>
OvRendering::HAL::NoneTextureHandle::TTextureHandle(Settings::ETextureType p_type) : m_context{
	.type = p_type,
}
{
}

template<>
OvRendering::HAL::NoneTextureHandle::TTextureHandle(Settings::ETextureType p_type, uint32_t p_id) : TTextureHandle(p_type)
{
}

template<>
void OvRendering::HAL::NoneTextureHandle::Bind(std::optional<uint32_t> p_slot) const
{
}

template<>
void OvRendering::HAL::NoneTextureHandle::Unbind() const
{
}

template<>
uint32_t OvRendering::HAL::NoneTextureHandle::GetID() const
{
	return 0;
}

template<>
OvRendering::Settings::ETextureType OvRendering::HAL::NoneTextureHandle::GetType() const
{
	return m_context.type;
}
