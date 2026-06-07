/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>
#include <string>

#include <OvCore/ECS/Components/AComponent.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector4.h>
#include <OvRendering/Data/Material.h>
#include <OvRendering/Resources/Font.h>
#include <OvRendering/Resources/Mesh.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	/**
	* Represents a renderable user interface text
	*/
	class CText : public AComponent
	{
	public:
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
		CText(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Sets the text content
		* @param p_text
		*/
		void SetText(const std::string& p_text);

		/**
		* Returns the text content
		*/
		const std::string& GetText() const;

		/**
		* Sets the font resource path
		* @param p_fontPath
		*/
		void SetFontPath(const std::string& p_fontPath);

		/**
		* Returns the font resource path
		*/
		const std::string& GetFontPath() const;

		/**
		* Sets the font size in canvas pixels
		* @param p_fontSize
		*/
		void SetFontSize(float p_fontSize);

		/**
		* Returns the font size in canvas pixels
		*/
		float GetFontSize() const;

		/**
		* Sets the text color
		* @param p_color
		*/
		void SetColor(const OvMaths::FVector4& p_color);

		/**
		* Returns the text color
		*/
		const OvMaths::FVector4& GetColor() const;

		/**
		* Sets the horizontal text alignment
		* @param p_alignment
		*/
		void SetHorizontalAlignment(EHorizontalAlignment p_alignment);

		/**
		* Returns the horizontal text alignment
		*/
		EHorizontalAlignment GetHorizontalAlignment() const;

		/**
		* Sets the vertical text alignment
		* @param p_alignment
		*/
		void SetVerticalAlignment(EVerticalAlignment p_alignment);

		/**
		* Returns the vertical text alignment
		*/
		EVerticalAlignment GetVerticalAlignment() const;

		/**
		* Returns the generated text mesh, or nullptr if the text cannot be rendered
		*/
		OvRendering::Resources::Mesh* GetMesh() const;

		/**
		* Returns the generated text mesh for a resolved UI size, or nullptr if the text cannot be rendered
		*/
		OvRendering::Resources::Mesh* GetMesh(const OvMaths::FVector2& p_resolvedSize) const;

		/**
		* Returns the generated text material, or nullptr if it cannot be initialized
		*/
		OvRendering::Data::Material* GetMaterial();

		/**
		* Returns the generated text bounds size
		*/
		const OvMaths::FVector2& GetSize() const;

		/**
		* Returns the generated text bounds size for a resolved UI size
		*/
		OvMaths::FVector2 GetSize(const OvMaths::FVector2& p_resolvedSize) const;

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
		OvRendering::Resources::Font* GetFont() const;
		void MarkMeshDirty();
		void RebuildMesh() const;
		void RebuildMesh(const OvMaths::FVector2& p_uiSize) const;
		void RefreshMaterial();

	private:
		std::string m_text = "Text";
		std::string m_fontPath = ":Fonts\\Roboto-Regular.ttf";
		float m_fontSize = 32.0f;
		OvMaths::FVector4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		EHorizontalAlignment m_horizontalAlignment = EHorizontalAlignment::LEFT;
		EVerticalAlignment m_verticalAlignment = EVerticalAlignment::TOP;
		mutable std::string m_unavailableFontPath;

		mutable bool m_meshDirty = true;
		mutable OvMaths::FVector2 m_lastUISize = OvMaths::FVector2::Zero;
		mutable OvMaths::FVector2 m_size = OvMaths::FVector2::Zero;
		mutable std::unique_ptr<OvRendering::Resources::Mesh> m_mesh;
		std::unique_ptr<OvRendering::Data::Material> m_material;
	};
}

namespace OvCore::ECS::Components
{
	template<>
	struct ComponentTraits<OvCore::ECS::Components::UI::CText>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::UI::CText";
	};
}
