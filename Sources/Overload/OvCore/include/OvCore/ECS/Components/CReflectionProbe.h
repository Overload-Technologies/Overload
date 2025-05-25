/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/AComponent.h>
#include <OvRendering/HAL/Framebuffer.h>
#include <OvRendering/HAL/Texture.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* A simple light that has no attenuation and that has a direction
	*/
	class CReflectionProbe : public AComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CReflectionProbe(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Sets the size of the reflection probe volume
		* @param p_size
		*/
		void SetSize(const OvMaths::FVector3& p_size);

		/**
		* Returns the size of the reflection probe volume
		*/
		const OvMaths::FVector3& GetSize() const;

		/**
		* Returns the cubemap texture
		*/
		std::shared_ptr<OvRendering::HAL::Texture> GetCubemap() const;

		/**
		* Returns the framebuffer used for rendering the cubemap
		*/
		OvRendering::HAL::Framebuffer& GetFramebuffer() const;

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
		std::unique_ptr<OvRendering::HAL::Framebuffer> m_framebuffer;
		std::shared_ptr<OvRendering::HAL::Texture> m_cubemap;
		OvMaths::FVector3 m_size;
	};
}
