/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>
#include <utility>
#include <vector>

#include <OvCore/ECS/Components/AComponent.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector4.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	/**
	* Arranges direct user interface children along an axis
	*/
	class CLayoutGroup : public AComponent
	{
	public:
		using ChildOffset = std::pair<const ECS::Actor*, OvMaths::FVector2>;

		struct ChildLayout
		{
			const ECS::Actor* actor = nullptr;
			OvMaths::FVector2 offset = OvMaths::FVector2::Zero;
			OvMaths::FVector2 size = OvMaths::FVector2::Zero;
			bool hasDirectWidth = false;
			bool hasDirectHeight = false;
			bool valid = false;
		};

		enum class EDirection
		{
			HORIZONTAL,
			VERTICAL
		};

		enum class EHorizontalAlignment
		{
			LEFT,
			CENTER,
			RIGHT
		};

		enum class EVerticalAlignment
		{
			TOP,
			CENTER,
			BOTTOM
		};

		/**
		* Constructor
		* @param p_owner
		*/
		CLayoutGroup(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Sets the layout direction
		* @param p_direction
		*/
		virtual void SetDirection(EDirection p_direction);

		/**
		* Returns the layout direction
		*/
		EDirection GetDirection() const;

		/**
		* Sets the non-negative spacing between children
		* @param p_spacing
		*/
		void SetSpacing(float p_spacing);

		/**
		* Returns the non-negative spacing between children
		*/
		float GetSpacing() const;

		/**
		* Returns the actual layout container size after children, padding and Transform UI size are resolved
		*/
		OvMaths::FVector2 GetComputedSize() const;

		/**
		* Sets the layout padding as left, right, top, bottom
		* @param p_padding
		*/
		void SetPadding(const OvMaths::FVector4& p_padding);

		/**
		* Returns the layout padding as left, right, top, bottom
		*/
		const OvMaths::FVector4& GetPadding() const;

		/**
		* Sets the horizontal children alignment
		* @param p_alignment
		*/
		void SetHorizontalAlignment(EHorizontalAlignment p_alignment);

		/**
		* Returns the horizontal children alignment
		*/
		EHorizontalAlignment GetHorizontalAlignment() const;

		/**
		* Sets the vertical children alignment
		* @param p_alignment
		*/
		void SetVerticalAlignment(EVerticalAlignment p_alignment);

		/**
		* Returns the vertical children alignment
		*/
		EVerticalAlignment GetVerticalAlignment() const;

		/**
		* Sets whether the layout should control children width
		* @param p_controlChildrenWidth
		*/
		void SetControlChildrenWidth(bool p_controlChildrenWidth);

		/**
		* Returns whether the layout controls children width
		*/
		bool GetControlChildrenWidth() const;

		/**
		* Sets whether the layout should control children height
		* @param p_controlChildrenHeight
		*/
		void SetControlChildrenHeight(bool p_controlChildrenHeight);

		/**
		* Returns whether the layout controls children height
		*/
		bool GetControlChildrenHeight() const;

		/**
		* Sets whether the layout should expand children width to fill available space
		* @param p_forceExpandWidth
		*/
		void SetForceExpandWidth(bool p_forceExpandWidth);

		/**
		* Returns whether the layout expands children width
		*/
		bool GetForceExpandWidth() const;

		/**
		* Sets whether the layout should expand children height to fill available space
		* @param p_forceExpandHeight
		*/
		void SetForceExpandHeight(bool p_forceExpandHeight);

		/**
		* Returns whether the layout expands children height
		*/
		bool GetForceExpandHeight() const;

		/**
		* Returns the layout offset for a direct child
		* @param p_child
		*/
		OvMaths::FVector2 GetChildOffset(const ECS::Actor& p_child) const;

		/**
		* Returns the resolved layout data for a direct child
		* @param p_child
		*/
		std::optional<ChildLayout> GetChildLayout(const ECS::Actor& p_child) const;

		/**
		* Returns the layout offsets for direct children
		*/
		std::vector<ChildOffset> GetChildOffsets() const;

		/**
		* Returns the resolved layout data for direct children
		*/
		std::vector<ChildLayout> GetChildLayouts() const;

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
		virtual bool IsDirectionEditable() const;

	private:
		struct LayoutInputChild
		{
			ECS::Actor* actor = nullptr;
			OvMaths::FVector2 preferredSize = OvMaths::FVector2::Zero;
		};

		struct LayoutCacheChildSignature
		{
			const ECS::Actor* actor = nullptr;
			OvMaths::FVector2 preferredSize = OvMaths::FVector2::Zero;
		};

		struct LayoutCacheSignature
		{
			EDirection direction = EDirection::HORIZONTAL;
			float spacing = 0.0f;
			OvMaths::FVector4 padding = OvMaths::FVector4::Zero;
			EHorizontalAlignment horizontalAlignment = EHorizontalAlignment::CENTER;
			EVerticalAlignment verticalAlignment = EVerticalAlignment::CENTER;
			bool controlChildrenWidth = false;
			bool controlChildrenHeight = false;
			bool forceExpandWidth = false;
			bool forceExpandHeight = false;
			OvMaths::FVector2 containerSize = OvMaths::FVector2::Zero;
			OvMaths::FVector2 pivot = OvMaths::FVector2::Zero;
			std::vector<LayoutCacheChildSignature> children;
		};

		struct LayoutCacheInput
		{
			LayoutCacheSignature signature;
			std::vector<LayoutInputChild> children;
		};

		struct LayoutCache
		{
			bool valid = false;
			LayoutCacheSignature signature;
			OvMaths::FVector2 size = OvMaths::FVector2::Zero;
			std::vector<ChildLayout> children;
		};

		LayoutCacheInput BuildLayoutCacheInput() const;
		const LayoutCache& GetResolvedLayout() const;
		void InvalidateLayoutCache() const;
		static bool HasSameLayoutSignature(const LayoutCacheSignature& p_lhs, const LayoutCacheSignature& p_rhs);

		EDirection m_direction = EDirection::HORIZONTAL;
		float m_spacing = 0.0f;
		OvMaths::FVector4 m_padding = OvMaths::FVector4::Zero;
		EHorizontalAlignment m_horizontalAlignment = EHorizontalAlignment::CENTER;
		EVerticalAlignment m_verticalAlignment = EVerticalAlignment::CENTER;
		bool m_controlChildrenWidth = false;
		bool m_controlChildrenHeight = false;
		bool m_forceExpandWidth = false;
		bool m_forceExpandHeight = false;
		mutable LayoutCache m_layoutCache;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CLayoutGroup>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CLayoutGroup";
	};
}
