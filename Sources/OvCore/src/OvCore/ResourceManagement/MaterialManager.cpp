/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ResourceManagement/MaterialManager.h"

OvCore::Resources::Material * OvCore::ResourceManagement::MaterialManager::CreateResource(const std::filesystem::path & p_path)
{
	std::string realPath = GetRealPath(p_path).string();

	Resources::Material* material = OvCore::Resources::Loaders::MaterialLoader::Create(realPath);
	if (material)
	{
		const_cast<std::string&>(material->path) = p_path.string(); // Force the resource path to fit the given path
	}

	return material;
}

void OvCore::ResourceManagement::MaterialManager::DestroyResource(OvCore::Resources::Material * p_resource)
{
	OvCore::Resources::Loaders::MaterialLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::MaterialManager::ReloadResource(OvCore::Resources::Material* p_resource, const std::filesystem::path& p_path)
{
	std::string realPath = GetRealPath(p_path).string();
	OvCore::Resources::Loaders::MaterialLoader::Reload(*p_resource, realPath);
}
