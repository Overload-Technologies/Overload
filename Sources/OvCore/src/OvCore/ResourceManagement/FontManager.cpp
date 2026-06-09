/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ResourceManagement/FontManager.h>

OvRendering::Resources::Font* OvCore::ResourceManagement::FontManager::CreateResource(const std::filesystem::path& p_path)
{
	auto* font = new OvRendering::Resources::Font(p_path.string(), GetRealPath(p_path));
	if (!font->IsValid())
	{
		delete font;
		return nullptr;
	}

	return font;
}

void OvCore::ResourceManagement::FontManager::DestroyResource(OvRendering::Resources::Font* p_resource)
{
	delete p_resource;
}

void OvCore::ResourceManagement::FontManager::ReloadResource(OvRendering::Resources::Font* p_resource, const std::filesystem::path& p_path)
{
	p_resource->Reload(GetRealPath(p_path));
}
