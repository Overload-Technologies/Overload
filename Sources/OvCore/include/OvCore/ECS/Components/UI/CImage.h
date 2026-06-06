/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <OvCore/ECS/Components/AComponent.h>
#include <OvCore/Resources/Material.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector4.h>
#include <OvRendering/Resources/Mesh.h>
#include <OvRendering/Resources/Texture.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	/**
	* Represents a renderable user interface image
	*/
	class CImage : public AComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CImage(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Sets the texture rendered by the image
		* @param p_texture
		*/
		void SetTexture(OvRendering::Resources::Texture* p_texture);

		/**
		* Returns the texture rendered by the image
		*/
		OvRendering::Resources::Texture* GetTexture() const;

		/**
		* Sets the image size
		* @param p_size
		*/
		void SetSize(const OvMaths::FVector2& p_size);

		/**
		* Returns the image size
		*/
		const OvMaths::FVector2& GetSize() const;

		/**
		* Sets the image tint
		* @param p_tint
		*/
		void SetTint(const OvMaths::FVector4& p_tint);

		/**
		* Returns the image tint
		*/
		const OvMaths::FVector4& GetTint() const;

		/**
		* Returns the generated quad mesh
		*/
		OvRendering::Resources::Mesh& GetMesh() const;

		/**
		* Returns the generated UI image material, or nullptr if it cannot be initialized
		*/
		OvCore::Resources::Material* GetMaterial();

		/**
		* Serialize the component
		* @param p_doc
		* @param p_node
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Deserialize the component
		* @param p_doc
		* @param p_node
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Defines how the component should be drawn in the inspector
		* @param p_root
		*/
		virtual void OnInspector(OvUI::Internal::WidgetContainer& p_root) override;

	private:
		void ValidateTextureReference();
		void RebuildMesh();
		void RefreshMaterial();
		void SyncTransformUISizeIfUnset();

	private:
		OvRendering::Resources::Texture* m_texture = nullptr;
		OvMaths::FVector2 m_size = { 100.0f, 100.0f };
		OvMaths::FVector4 m_tint = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::unique_ptr<OvRendering::Resources::Mesh> m_mesh;
		std::unique_ptr<OvCore::Resources::Material> m_material;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CImage>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CImage";
	};
}
