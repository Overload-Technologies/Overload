/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/AComponent.h>
#include <OvMaths/FVector2.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	/**
	* Represents a root canvas for in-game user interface elements
	*/
	class CCanvas : public AComponent
	{
	public:
		enum class EScalerMode
		{
			CONSTANT_PIXEL_SIZE,
			SCALE_WITH_SCREEN_SIZE
		};

		enum class EScreenMatchMode
		{
			MATCH_WIDTH_OR_HEIGHT,
			EXPAND,
			SHRINK
		};

		/**
		* Constructor
		* @param p_owner
		*/
		CCanvas(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Sets the canvas reference resolution
		* @param p_referenceResolution
		*/
		void SetReferenceResolution(const OvMaths::FVector2& p_referenceResolution);

		/**
		* Returns the canvas reference resolution
		*/
		const OvMaths::FVector2& GetReferenceResolution() const;

		/**
		* Sets the canvas scale factor
		* @param p_scaleFactor
		*/
		void SetScaleFactor(float p_scaleFactor);

		/**
		* Returns the canvas scale factor
		*/
		float GetScaleFactor() const;

		/**
		* Sets the number of UI pixels represented by one world unit
		* @param p_pixelsPerUnit
		*/
		void SetPixelsPerUnit(float p_pixelsPerUnit);

		/**
		* Returns the number of UI pixels represented by one world unit
		*/
		float GetPixelsPerUnit() const;

		/**
		* Sets the canvas scaler mode
		* @param p_scalerMode
		*/
		void SetScalerMode(EScalerMode p_scalerMode);

		/**
		* Returns the canvas scaler mode
		*/
		EScalerMode GetScalerMode() const;

		/**
		* Sets the screen match mode used when scaler mode is Scale With Screen Size
		* @param p_screenMatchMode
		*/
		void SetScreenMatchMode(EScreenMatchMode p_screenMatchMode);

		/**
		* Returns the screen match mode used when scaler mode is Scale With Screen Size
		*/
		EScreenMatchMode GetScreenMatchMode() const;

		/**
		* Sets the width/height match factor in range [0, 1]
		* @param p_matchWidthOrHeight
		*/
		void SetMatchWidthOrHeight(float p_matchWidthOrHeight);

		/**
		* Returns the width/height match factor in range [0, 1]
		*/
		float GetMatchWidthOrHeight() const;

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
		OvMaths::FVector2 m_referenceResolution = { 1920.0f, 1080.0f };
		float m_scaleFactor = 1.0f;
		float m_pixelsPerUnit = 100.0f;
		EScalerMode m_scalerMode = EScalerMode::SCALE_WITH_SCREEN_SIZE;
		EScreenMatchMode m_screenMatchMode = EScreenMatchMode::MATCH_WIDTH_OR_HEIGHT;
		float m_matchWidthOrHeight = 0.5f;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CCanvas>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CCanvas";
	};
}
