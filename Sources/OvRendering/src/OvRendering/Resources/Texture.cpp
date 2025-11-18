/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/Resources/Texture.h>

OvRendering::HAL::Texture& OvRendering::Resources::Texture::GetTexture()
{
	OVASSERT(m_texture != nullptr, "Trying to access a null Texture");
	return *m_texture;
}

OvRendering::Resources::Texture::Texture(const std::string p_path, std::unique_ptr<HAL::Texture>&& p_texture) : path(p_path)
{
	SetTexture(std::move(p_texture));
}

void OvRendering::Resources::Texture::SetTexture(std::unique_ptr<HAL::Texture>&& p_texture)
{
	OVASSERT(p_texture != nullptr, "Cannot assign an invalid texture!");
	m_texture = std::move(p_texture);
}
