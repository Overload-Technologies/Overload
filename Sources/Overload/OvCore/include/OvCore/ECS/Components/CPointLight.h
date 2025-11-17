/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Entities/Light.h>

#include "OvCore/ECS/Components/CLight.h"

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* A light that is localized and has attenuation
	*/
	class CPointLight : public CLight
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CPointLight(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the light constant
		*/
		float GetConstant() const;

		/**
		* Returns the light linear
		*/
		float GetLinear() const;

		/**
		* Returns the light quadratic
		*/
		float GetQuadratic() const;

		/**
		* Defines the light constant
		* @param p_constant
		*/
		void SetConstant(float p_constant);

		/**
		* Defines the light linear
		* @param p_linear
		*/
		void SetLinear(float p_linear);

		/**
		* Defines the light quadratic
		* @param p_quadratic
		*/
		void SetQuadratic(float p_quadratic);

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
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CPointLight>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CPointLight";
	};
}