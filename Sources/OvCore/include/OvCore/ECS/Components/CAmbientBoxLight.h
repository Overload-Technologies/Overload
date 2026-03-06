/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvCore/ECS/Components/CLight.h"

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* Represents an ambient light with a box shape
	*/
	class CAmbientBoxLight : public CLight
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CAmbientBoxLight(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Returns the size of the box
		*/
		OvMaths::FVector3 GetSize() const;

		/**
		* Defines the size of the box
		* @param p_size
		*/
		void SetSize(const OvMaths::FVector3& p_size);

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
	struct ComponentTraits<OvCore::ECS::Components::CAmbientBoxLight>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CAmbientBoxLight";
	};
}