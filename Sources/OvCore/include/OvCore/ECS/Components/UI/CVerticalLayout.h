/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/UI/CLayoutGroup.h>

namespace OvCore::ECS::Components::UI
{
	/**
	* Arranges direct user interface children vertically
	*/
	class CVerticalLayout : public CLayoutGroup
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CVerticalLayout(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		std::string GetTypeName() override;

		/**
		* Keeps the vertical layout direction
		*/
		void SetDirection(EDirection p_direction) override;

	protected:
		bool IsDirectionEditable() const override;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CVerticalLayout>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CVerticalLayout";
	};
}

