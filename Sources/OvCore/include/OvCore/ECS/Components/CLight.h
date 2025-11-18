/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Entities/Light.h>

#include "OvCore/ECS/Components/AComponent.h"

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* Base class for any light
	*/
	class CLight : public AComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CLight(ECS::Actor& p_owner);

		/**
		* Returns light data
		*/
		OvRendering::Entities::Light& GetData();

		/**
		* Returns light color
		*/
		const OvMaths::FVector3& GetColor() const;

		/**
		* Returns light intensity
		*/
		float GetIntensity() const;

		/**
		* Defines a new color for the light
		* @param p_color
		*/
		void SetColor(const OvMaths::FVector3& p_color);

		/**
		* Defines the intensity for the light
		* @param p_intensity
		*/
		void SetIntensity(float p_intensity);

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

	protected:
		OvRendering::Entities::Light m_data;
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CLight>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CLight";
	};
}