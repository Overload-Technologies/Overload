/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "AComponent.h"

#include <OvCore/Rendering/PostProcess/PostProcessStack.h>
#include <OvCore/Rendering/PostProcess/BloomEffect.h>
#include <OvCore/Rendering/PostProcess/FXAAEffect.h>
#include <OvCore/Rendering/PostProcess/TonemappingEffect.h>
#include <OvCore/Rendering/PostProcess/AutoExposureEffect.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* A component that handle a post process stack
	*/
	class CPostProcessStack : public AComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CPostProcessStack(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

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

		/**
		* Returns the post process setting stack
		*/
		const OvCore::Rendering::PostProcess::PostProcessStack& GetStack();

		/**
		* Returns the bloom settings
		*/
		const OvCore::Rendering::PostProcess::BloomSettings& GetBloomSettings() const;

		/**
		* Returns the auto-exposure settings
		*/
		const OvCore::Rendering::PostProcess::AutoExposureSettings& GetAutoExposureSettings() const;

		/**
		* Returns the tonemapping settings
		*/
		const OvCore::Rendering::PostProcess::TonemappingSettings& GetTonemappingSettings() const;

		/**
		* Returns the FXAA settings
		*/
		const OvCore::Rendering::PostProcess::FXAASettings& GetFXAASettings() const;

		/**
		* Sets the bloom settings
		* * @param p_settings
		*/
		void SetBloomSettings(const OvCore::Rendering::PostProcess::BloomSettings& p_settings);

		/**
		* Sets the auto exposure settings
		* * @param p_settings
		*/
		void SetAutoExposureSettings(const OvCore::Rendering::PostProcess::AutoExposureSettings& p_settings);

		/**
		* Sets the tonemapping settings
		* @param p_settings
		*/
		void SetTonemappingSettings(const OvCore::Rendering::PostProcess::TonemappingSettings& p_settings);

		/**
		* Sets the FXAA settings
		* @param p_settings
		*/
		void SetFXAASettings(const OvCore::Rendering::PostProcess::FXAASettings& p_settings);

	private:
		OvCore::Rendering::PostProcess::PostProcessStack m_settings;
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CPostProcessStack>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CPostProcessStack";
	};
}