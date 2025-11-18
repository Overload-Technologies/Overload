/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

namespace OvEditor::Core
{
	/**
	* Handle the creation and storage of editor specific resources
	*/
	class EditorResources
	{
	public:
		/**
		* Constructor
		* @param p_editorAssetsPath
		*/
		EditorResources(const std::string& p_editorAssetsPath);

		/**
		* Destructor
		*/
		~EditorResources();

		/**
		* Returns the file icon identified by the given string or nullptr on fail
		* @param p_filename
		*/
		OvRendering::Resources::Texture* GetFileIcon(const std::string& p_filename);

		/**
		* Returns the texture identified by the given string or nullptr on fail
		* @param p_id
		*/
		OvRendering::Resources::Texture* GetTexture(const std::string& p_id);

		/**
		* Returns the model identified by the given string or nullptr on fail
		* @param p_id
		*/
		OvRendering::Resources::Model* GetModel(const std::string& p_id);

		/**
		* Returns the shader identified by the given string or nullptr on fail
		* @param p_id
		*/
		OvRendering::Resources::Shader* GetShader(const std::string& p_id);

	private:
		std::unordered_map<std::string, OvRendering::Resources::Texture*> m_textures;
		std::unordered_map<std::string, OvRendering::Resources::Model*> m_models;
		std::unordered_map<std::string, OvRendering::Resources::Shader*> m_shaders;
	};
}