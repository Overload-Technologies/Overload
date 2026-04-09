/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Geometry/Vertex.h>
#include <OvRendering/Resources/Model.h>

#include "OvCore/ECS/Components/AComponent.h"

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* A ModelRenderer is necessary in combination with a MaterialRenderer to render a model in the world
	*/
	class CModelRenderer : public AComponent
	{
	public:
		/**
		* Defines how the model renderer bounding sphere should be interpreted
		*/
		enum class EFrustumBehaviour
		{
			DISABLED = 0,
			DEPRECATED_MODEL_BOUNDS = 1, // This is not used anymore, but the enum value is kept for compatibility
			MESH_BOUNDS = 2,
			CUSTOM_BOUNDS = 3
		};

		/**
		* Constructor
		* @param p_owner
		*/
		CModelRenderer(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Defines the model to use
		* @param p_model
		*/
		void SetModel(OvRendering::Resources::Model* p_model);

		/**
		* Returns the current model
		*/
		OvRendering::Resources::Model* GetModel() const;

		/**
		* Sets a bounding mode
		* @param p_boundingMode
		*/
		void SetFrustumBehaviour(EFrustumBehaviour p_boundingMode);

		/**
		* Returns the current bounding mode
		*/
		EFrustumBehaviour GetFrustumBehaviour() const;

		/**
		* Returns the custom bounding sphere
		*/
		const OvRendering::Geometry::BoundingSphere& GetCustomBoundingSphere() const;

		/**
		* Sets the custom bounding sphere
		* @param p_boundingSphere
		*/
		void SetCustomBoundingSphere(const OvRendering::Geometry::BoundingSphere& p_boundingSphere);

		/**
		* Returns the scale applied to skinned mesh bounds for frustum culling
		*/
		float GetSkinningBoundsScale() const;

		/**
		* Sets the scale applied to skinned mesh bounds for frustum culling
		* @param p_scale
		*/
		void SetSkinningBoundsScale(float p_scale);

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
		OvRendering::Resources::Model* m_model = nullptr;
		OvTools::Eventing::Event<> m_modelChangedEvent;
		OvRendering::Geometry::BoundingSphere m_customBoundingSphere = { {}, 1.0f };
		EFrustumBehaviour m_frustumBehaviour = EFrustumBehaviour::MESH_BOUNDS;
		float m_skinningBoundsScale = 1.5f;
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CModelRenderer>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CModelRenderer";
	};
}
