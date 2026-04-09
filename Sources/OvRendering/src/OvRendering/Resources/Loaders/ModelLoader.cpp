/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <utility>

#include "OvRendering/Resources/Loaders/ModelLoader.h"

OvRendering::Resources::Parsers::AssimpParser OvRendering::Resources::Loaders::ModelLoader::__ASSIMP;

OvRendering::Resources::Model* OvRendering::Resources::Loaders::ModelLoader::Create(
	const std::string& p_filepath,
	Parsers::EModelParserFlags p_parserFlags,
	bool p_generateEmbeddedAssets
)
{
	Model* result = new Model(p_filepath);

	if (__ASSIMP.LoadModel(
		p_filepath,
		result->m_meshes,
		result->m_materialNames,
		result->m_skeleton,
		result->m_animations,
		result->m_embeddedMaterials,
		result->m_embeddedTextures,
		p_parserFlags,
		p_generateEmbeddedAssets
	))
	{
		result->ComputeBoundingSphere();
		return result;
	}

	delete result;

	return nullptr;
}

void OvRendering::Resources::Loaders::ModelLoader::Reload(
	Model& p_model,
	const std::string& p_filePath,
	Parsers::EModelParserFlags p_parserFlags,
	bool p_generateEmbeddedAssets
)
{
	Model* newModel = Create(p_filePath, p_parserFlags, p_generateEmbeddedAssets);

	if (newModel)
	{
		p_model.m_meshes = newModel->m_meshes;
		p_model.m_materialNames = newModel->m_materialNames;
		p_model.m_embeddedMaterials = std::move(newModel->m_embeddedMaterials);
		p_model.m_embeddedTextures = std::move(newModel->m_embeddedTextures);
		p_model.m_skeleton = std::move(newModel->m_skeleton);
		p_model.m_animations = std::move(newModel->m_animations);
		p_model.m_boundingSphere = newModel->m_boundingSphere;
		newModel->m_meshes.clear();
		delete newModel;
	}
}

bool OvRendering::Resources::Loaders::ModelLoader::Destroy(Model*& p_modelInstance)
{
	if (p_modelInstance)
	{
		delete p_modelInstance;
		p_modelInstance = nullptr;

		return true;
	}

	return false;
}
