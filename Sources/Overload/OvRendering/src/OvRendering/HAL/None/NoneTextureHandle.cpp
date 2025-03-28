/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneTextureHandle.h>

template<>
OvRendering::HAL::NoneTextureHandle::TTextureHandle()
{
}

template<>
OvRendering::HAL::NoneTextureHandle::TTextureHandle(uint32_t p_id)
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
