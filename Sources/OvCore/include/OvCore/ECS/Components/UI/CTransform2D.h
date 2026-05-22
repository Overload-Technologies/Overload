/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/AComponent.h>
#include <OvMaths/FMatrix4.h>
#include <OvMaths/FVector2.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	/**
	* Represents 2D transformations applied to a user interface actor
	*/
	class CTransform2D : public AComponent
	{
	public:
		enum class EAnchorPreset
		{
			TOP_LEFT,
			TOP_CENTER,
			TOP_RIGHT,
			MIDDLE_LEFT,
			CENTER,
			MIDDLE_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_CENTER,
			BOTTOM_RIGHT
		};

		/**
		* Constructor
		* @param p_owner
		*/
		CTransform2D(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Sets the 2D position
		* @param p_position
		*/
		void SetPosition(const OvMaths::FVector2& p_position);

		/**
		* Returns the 2D position
		*/
		const OvMaths::FVector2& GetPosition() const;

		/**
		* Sets the 2D rotation in degrees
		* @param p_rotation
		*/
		void SetRotation(float p_rotation);

		/**
		* Returns the 2D rotation in degrees
		*/
		float GetRotation() const;

		/**
		* Sets the 2D scale
		* @param p_scale
		*/
		void SetScale(const OvMaths::FVector2& p_scale);

		/**
		* Returns the 2D scale
		*/
		const OvMaths::FVector2& GetScale() const;

		/**
		* Sets the anchor preset
		* @param p_anchorPreset
		*/
		void SetAnchorPreset(EAnchorPreset p_anchorPreset);

		/**
		* Returns the anchor preset
		*/
		EAnchorPreset GetAnchorPreset() const;

		/**
		* Returns a transform matrix resolved against a canvas size and layout offset
		* @param p_canvasSize
		* @param p_layoutOffset
		*/
		OvMaths::FMatrix4 GetMatrix(const OvMaths::FVector2& p_canvasSize, const OvMaths::FVector2& p_layoutOffset = OvMaths::FVector2::Zero) const;

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
		OvMaths::FVector2 GetAnchoredPosition(const OvMaths::FVector2& p_canvasSize, const OvMaths::FVector2& p_layoutOffset) const;

	private:
		OvMaths::FVector2 m_position = OvMaths::FVector2::Zero;
		float m_rotation = 0.0f;
		OvMaths::FVector2 m_scale = OvMaths::FVector2::One;
		EAnchorPreset m_anchorPreset = EAnchorPreset::CENTER;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CTransform2D>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CTransform2D";
	};
}
