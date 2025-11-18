/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <variant>

#include <OvCore/Resources/Material.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CModelRenderer.h>

#include "OvEditor/Panels/AViewControllable.h"

namespace OvEditor::Panels
{
	/**
	* Provide a view for assets
	*/
	class AssetView : public OvEditor::Panels::AViewControllable
	{
	public:
		using ViewableResource = std::variant<OvRendering::Resources::Model*, OvRendering::Resources::Texture*, OvCore::Resources::Material*>;

		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AssetView(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Returns the scene used by this view
		*/
		virtual OvCore::SceneSystem::Scene* GetScene();

		/**
		* Defines the resource to preview
		* @parma p_resource
		*/
		void SetResource(ViewableResource p_resource);

		/**
		* Clear any currently viewed resource
		*/
		void ClearResource();

		/**
		* Set the currently viewed resource to the given texture
		* @param p_texture
		*/
		void SetTexture(OvRendering::Resources::Texture& p_texture);

		/**
		* Set the currently viewed resource to the given model
		* @param p_model
		*/
		void SetModel(OvRendering::Resources::Model& p_model);

		/**
		* Set the currently viewed resource to the given material
		* @param p_material
		*/
		void SetMaterial(OvCore::Resources::Material& p_material);

		/**
		* Return the currently previewed resource
		*/
		const ViewableResource& GetResource() const;

	private:
		OvCore::Resources::Material m_defaultMaterial;
		OvCore::Resources::Material m_textureMaterial;

		OvCore::ECS::Actor* m_assetActor;
		OvCore::ECS::Components::CModelRenderer* m_modelRenderer;
		OvCore::ECS::Components::CMaterialRenderer* m_materialRenderer;
		ViewableResource m_resource;
		OvCore::SceneSystem::Scene m_scene;
	};
}