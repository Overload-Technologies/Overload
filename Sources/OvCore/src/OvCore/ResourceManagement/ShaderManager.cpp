/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ResourceManagement/ShaderManager.h"

OvRendering::Resources::Shader* OvCore::ResourceManagement::ShaderManager::CreateResource(const std::filesystem::path & p_path)
{
	std::string realPath = GetRealPath(p_path).string();
	auto pathParserCallback = [this](const std::string& s) { return GetRealPath(std::filesystem::path{s}).string(); };
	OvRendering::Resources::Shader* shader = OvRendering::Resources::Loaders::ShaderLoader::Create(realPath, pathParserCallback);
	if (shader)
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(shader) + offsetof(OvRendering::Resources::Shader, path)) = p_path.string(); // Force the resource path to fit the given path

	return shader;
}

void OvCore::ResourceManagement::ShaderManager::DestroyResource(OvRendering::Resources::Shader* p_resource)
{
	OvRendering::Resources::Loaders::ShaderLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::ShaderManager::ReloadResource(OvRendering::Resources::Shader* p_resource, const std::filesystem::path& p_path)
{
	auto pathParserCallback = [this](const std::string& s) { return GetRealPath(std::filesystem::path{s}).string(); };
	OvRendering::Resources::Loaders::ShaderLoader::Recompile(*p_resource, p_path.string(), pathParserCallback);
}
