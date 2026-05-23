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
	* Arranges direct user interface children horizontally
	*/
	class CHorizontalLayout : public CLayoutGroup
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CHorizontalLayout(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		std::string GetTypeName() override;

		/**
		* Keeps the horizontal layout direction
		*/
		void SetDirection(EDirection p_direction) override;

	protected:
		bool IsDirectionEditable() const override;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CHorizontalLayout>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CHorizontalLayout";
	};
}

