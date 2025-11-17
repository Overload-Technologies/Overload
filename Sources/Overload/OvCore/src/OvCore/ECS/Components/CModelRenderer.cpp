/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>

#include <OvUI/Plugins/DDTarget.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Texts/TextColored.h>
#include <OvUI/Widgets/Visual/Image.h>

OvCore::ECS::Components::CModelRenderer::CModelRenderer(ECS::Actor& p_owner) : AComponent(p_owner)
{
	m_modelChangedEvent += [this]
	{
		if (auto materialRenderer = owner.GetComponent<CMaterialRenderer>())
			materialRenderer->UpdateMaterialList();
	};
}

std::string OvCore::ECS::Components::CModelRenderer::GetName()
{
	return "Model Renderer";
}

std::string OvCore::ECS::Components::CModelRenderer::GetTypeName()
{
	return std::string{ComponentTraits<CModelRenderer>::Name};
}

void OvCore::ECS::Components::CModelRenderer::SetModel(OvRendering::Resources::Model* p_model)
{
	m_model = p_model;
	m_modelChangedEvent.Invoke();
}

OvRendering::Resources::Model * OvCore::ECS::Components::CModelRenderer::GetModel() const
{
	return m_model;
}

void OvCore::ECS::Components::CModelRenderer::SetFrustumBehaviour(EFrustumBehaviour p_boundingMode)
{
	m_frustumBehaviour = p_boundingMode;
}

OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour OvCore::ECS::Components::CModelRenderer::GetFrustumBehaviour() const
{
	return m_frustumBehaviour;
}

const OvRendering::Geometry::BoundingSphere& OvCore::ECS::Components::CModelRenderer::GetCustomBoundingSphere() const
{
	return m_customBoundingSphere;
}

void OvCore::ECS::Components::CModelRenderer::SetCustomBoundingSphere(const OvRendering::Geometry::BoundingSphere& p_boundingSphere)
{
	m_customBoundingSphere = p_boundingSphere;
}

void OvCore::ECS::Components::CModelRenderer::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	OvCore::Helpers::Serializer::SerializeModel(p_doc, p_node, "model", m_model);
	OvCore::Helpers::Serializer::SerializeInt(p_doc, p_node, "frustum_behaviour", reinterpret_cast<int&>(m_frustumBehaviour));
	OvCore::Helpers::Serializer::SerializeVec3(p_doc, p_node, "custom_bounding_sphere_position", m_customBoundingSphere.position);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "custom_bounding_sphere_radius", m_customBoundingSphere.radius);
}

void OvCore::ECS::Components::CModelRenderer::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::DeserializeModel(p_doc, p_node, "model", m_model);
	OvCore::Helpers::Serializer::DeserializeInt(p_doc, p_node, "frustum_behaviour", reinterpret_cast<int&>(m_frustumBehaviour));
	OvCore::Helpers::Serializer::DeserializeVec3(p_doc, p_node, "custom_bounding_sphere_position", m_customBoundingSphere.position);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "custom_bounding_sphere_radius", m_customBoundingSphere.radius);
}

void OvCore::ECS::Components::CModelRenderer::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	GUIDrawer::DrawMesh(p_root, "Model", m_model, &m_modelChangedEvent);

	GUIDrawer::CreateTitle(p_root, "Frustum Culling Behaviour");
	auto& boundingMode = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(0);
	boundingMode.choices.emplace(0, "Disabled");
	// [[deprecated]] boundingMode.choices.emplace(1, "Model Bounds");
	boundingMode.choices.emplace(2, "Mesh Bounds");
	boundingMode.choices.emplace(3, "Custom Bounds");
	auto& boundingModeDispatcher = boundingMode.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	boundingModeDispatcher.RegisterReference(reinterpret_cast<int&>(m_frustumBehaviour));

	auto& centerLabel = p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Bounding Sphere Center", GUIDrawer::TitleColor);
	auto& centerWidget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 3>>(GUIDrawer::GetDataType<float>(), GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT, 0.f, 0.05f, "", GUIDrawer::GetFormat<float>());
	auto& centerDispatcher = centerWidget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 3>>>();
	centerDispatcher.RegisterReference(reinterpret_cast<std::array<float, 3>&>(m_customBoundingSphere.position));

	auto& radiusLabel = p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Bounding Sphere Radius", GUIDrawer::TitleColor);
	auto& radiusWidget = p_root.CreateWidget<OvUI::Widgets::Drags::DragFloat>(0.0f, GUIDrawer::_MAX_FLOAT, 0.f, 0.1f);
	auto& radiusDispatcher = radiusWidget.AddPlugin<OvUI::Plugins::DataDispatcher<float>>();
	radiusDispatcher.RegisterReference(m_customBoundingSphere.radius);

	boundingMode.ValueChangedEvent += [&](int p_choice)
	{
		centerLabel.enabled = centerWidget.enabled = radiusLabel.enabled = radiusWidget.enabled = p_choice == 3;
	};

	centerLabel.enabled = centerWidget.enabled = radiusLabel.enabled = radiusWidget.enabled = m_frustumBehaviour == EFrustumBehaviour::CUSTOM_BOUNDS;
}
