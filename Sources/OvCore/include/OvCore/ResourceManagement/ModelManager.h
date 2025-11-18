/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Resources/Loaders/ModelLoader.h>

#include "OvCore/ResourceManagement/AResourceManager.h"

namespace OvCore::ResourceManagement
{
	/**
	* ResourceManager of models
	*/
	class ModelManager : public AResourceManager<OvRendering::Resources::Model>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual OvRendering::Resources::Model* CreateResource(const std::filesystem::path & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(OvRendering::Resources::Model* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(OvRendering::Resources::Model* p_resource, const std::filesystem::path& p_path) override;
	};
}