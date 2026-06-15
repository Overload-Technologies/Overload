/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>

#include "OvCore/ECS/Components/AComponent.h"

#include <OvMaths/FMatrix4.h>
#include <OvMaths/FTransform.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FQuaternion.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* Represents the 3D transformations applied to an actor
	*/
	class CTransform : public AComponent
	{
	public:
		enum class EUIAnchorPreset
		{
			TOP_LEFT,
			TOP_CENTER,
			TOP_RIGHT,
			MIDDLE_LEFT,
			CENTER,
			MIDDLE_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_CENTER,
			BOTTOM_RIGHT,
			HORIZONTAL_STRETCH_TOP,
			HORIZONTAL_STRETCH_MIDDLE,
			HORIZONTAL_STRETCH_BOTTOM,
			VERTICAL_STRETCH_LEFT,
			VERTICAL_STRETCH_CENTER,
			VERTICAL_STRETCH_RIGHT,
			STRETCH_BOTH
		};

		struct UIData
		{
			OvMaths::FVector2 position = OvMaths::FVector2::Zero;
			OvMaths::FVector2 size = OvMaths::FVector2::Zero;
			OvMaths::FVector2 pivot = OvMaths::FVector2::Zero;
			EUIAnchorPreset anchorPreset = EUIAnchorPreset::CENTER;
		};

		/**
		* Create a transform without setting a parent
		* @param p_localPosition
		* @param p_localRotation
		* @param p_localScale
		*/
		CTransform(ECS::Actor& p_owner, struct OvMaths::FVector3 p_localPosition = OvMaths::FVector3(0.0f, 0.0f, 0.0f), OvMaths::FQuaternion p_localRotation = OvMaths::FQuaternion::Identity, struct OvMaths::FVector3 p_localScale = OvMaths::FVector3(1.0f, 1.0f, 1.0f));

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Defines a parent to the transform
		* @param p_parent
		*/
		void SetParent(CTransform& p_parent);

		/**
		* Set the parent to nullptr and recalculate world matrix
		* Returns true on success
		*/
		bool RemoveParent();

		/**
		* Check if the transform has a parent
		*/
		bool HasParent() const;

		/**
		* Set the position of the transform in the local space
		* @param p_newPosition
		*/
		void SetLocalPosition(struct OvMaths::FVector3 p_newPosition);

		/**
		* Set the rotation of the transform in the local space
		* @param p_newRotation
		*/
		void SetLocalRotation(OvMaths::FQuaternion p_newRotation);

		/**
		* Set the scale of the transform in the local space
		* @param p_newScale
		*/
		void SetLocalScale(struct OvMaths::FVector3 p_newScale);


		/**
		* Set the position of the transform in world space
		* @param p_newPosition
		*/
		void SetWorldPosition(struct OvMaths::FVector3 p_newPosition);

		/**
		* Set the rotation of the transform in world space
		* @param p_newRotation
		*/
		void SetWorldRotation(OvMaths::FQuaternion p_newRotation);

		/**
		* Set the scale of the transform in world space
		* @param p_newScale
		*/
		void SetWorldScale(struct OvMaths::FVector3 p_newScale);

		/**
		* Translate in the local space
		* @param p_translation
		*/
		void TranslateLocal(const struct OvMaths::FVector3& p_translation);

		/**
		* Rotate in the local space
		* @param p_rotation
		*/
		void RotateLocal(const OvMaths::FQuaternion& p_rotation);

		/**
		* Scale in the local space
		* @param p_scale
		*/
		void ScaleLocal(const struct OvMaths::FVector3& p_scale);

		/**
		* Return the position in local space
		*/
		const OvMaths::FVector3& GetLocalPosition() const;

		/**
		* Return the rotation in local space
		*/
		const OvMaths::FQuaternion& GetLocalRotation() const;

		/**
		* Return the scale in local space
		*/
		const OvMaths::FVector3& GetLocalScale() const;

		/**
		* Return the position in world space
		*/
		const OvMaths::FVector3& GetWorldPosition() const;

		/**
		* Return the rotation in world space
		*/
		const OvMaths::FQuaternion& GetWorldRotation() const;

		/**
		* Return the scale in world space
		*/
		const OvMaths::FVector3& GetWorldScale() const;

		/**
		* Return the local matrix
		*/
		const OvMaths::FMatrix4& GetLocalMatrix() const;

		/**
		* Return the world matrix
		*/
		const OvMaths::FMatrix4& GetWorldMatrix() const;

		/**
		* Return the FTransform attached to the CTransform
		*/
		OvMaths::FTransform& GetFTransform();

		/**
		* Return the transform world forward
		*/
		OvMaths::FVector3 GetWorldForward() const;

		/**
		* Return the transform world up
		*/
		OvMaths::FVector3 GetWorldUp() const;

		/**
		* Return the transform world right
		*/
		OvMaths::FVector3 GetWorldRight() const;

		/**
		* Return the transform local forward
		*/
		OvMaths::FVector3 GetLocalForward() const;

		/**
		* Return the transform local up
		*/
		OvMaths::FVector3 GetLocalUp() const;

		/**
		* Return the transform local right
		*/
		OvMaths::FVector3 GetLocalRight() const;

		void EnableUIData();
		void DisableUIData();
		bool HasUIData() const;
		bool HasActiveUIData() const;
		const std::optional<UIData>& GetUIData() const;

		void SetUIPosition(const OvMaths::FVector2& p_position);
		const OvMaths::FVector2& GetUIPosition() const;

		void SetUIRotation(float p_rotation);
		float GetUIRotation() const;

		void SetUIScale(const OvMaths::FVector2& p_scale);
		OvMaths::FVector2 GetUIScale() const;

		void SetUISize(const OvMaths::FVector2& p_size);
		const OvMaths::FVector2& GetUISize() const;

		void SetUIPivot(const OvMaths::FVector2& p_pivot);
		const OvMaths::FVector2& GetUIPivot() const;

		void SetUIAnchorPreset(EUIAnchorPreset p_anchorPreset);
		EUIAnchorPreset GetUIAnchorPreset() const;

		bool IsHorizontalUIPositionEditable() const;
		bool IsVerticalUIPositionEditable() const;

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
		static EUIAnchorPreset ToUIAnchorPreset(int p_value);
		static bool IsHorizontalUIPositionEditable(EUIAnchorPreset p_anchorPreset);
		static bool IsVerticalUIPositionEditable(EUIAnchorPreset p_anchorPreset);

		UIData& GetOrCreateUIData();
		const UIData& GetUIDataOrDefault() const;

	private:
		OvMaths::FTransform m_transform;
		std::optional<UIData> m_uiData;
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CTransform>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CTransform";
	};
}
