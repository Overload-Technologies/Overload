/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <format>
#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>

#include <OvUI/Widgets/Layout/Dummy.h>
#include <OvUI/Widgets/InputFields/AssetField.h>
#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Visual/Separator.h>

OvCore::ECS::Components::CMaterialRenderer::CMaterialRenderer(ECS::Actor & p_owner) : AComponent(p_owner)
{
	m_materials.fill(nullptr);

	for (uint8_t i = 0; i < kMaxMaterialCount; ++i)
		m_materialFields[i].fill(nullptr);

	UpdateMaterialList();
}

std::string OvCore::ECS::Components::CMaterialRenderer::GetName()
{
	return "Material Renderer";
}

std::string OvCore::ECS::Components::CMaterialRenderer::GetTypeName()
{
	return std::string{ComponentTraits<CMaterialRenderer>::Name};
}

void OvCore::ECS::Components::CMaterialRenderer::FillWithMaterial(OvCore::Resources::Material & p_material)
{
	for (uint8_t i = 0; i < m_materials.size(); ++i)
		m_materials[i] = &p_material;
}

void OvCore::ECS::Components::CMaterialRenderer::SetMaterialAtIndex(uint8_t p_index, OvCore::Resources::Material& p_material)
{
	m_materials[p_index] = &p_material;
}

OvCore::Resources::Material* OvCore::ECS::Components::CMaterialRenderer::GetMaterialAtIndex(uint8_t p_index)
{
	return m_materials.at(p_index);
}

void OvCore::ECS::Components::CMaterialRenderer::RemoveMaterialAtIndex(uint8_t p_index)
{
	if (p_index < m_materials.size())
	{
		m_materials[p_index] = nullptr;;
	}
}

void OvCore::ECS::Components::CMaterialRenderer::RemoveMaterialByInstance(OvCore::Resources::Material& p_instance)
{
	for (uint8_t i = 0; i < m_materials.size(); ++i)
		if (m_materials[i] == &p_instance)
			m_materials[i] = nullptr;
}

void OvCore::ECS::Components::CMaterialRenderer::RemoveAllMaterials()
{
	for (uint8_t i = 0; i < m_materials.size(); ++i)
		m_materials[i] = nullptr;
}

const OvMaths::FMatrix4 & OvCore::ECS::Components::CMaterialRenderer::GetUserMatrix() const
{
	return m_userMatrix;
}

const OvCore::ECS::Components::CMaterialRenderer::MaterialList& OvCore::ECS::Components::CMaterialRenderer::GetMaterials() const
{
	return m_materials;
}

void OvCore::ECS::Components::CMaterialRenderer::SetVisibilityFlags(OvCore::Rendering::EVisibilityFlags p_flags)
{
	m_visibilityFlags = p_flags;
}

OvCore::Rendering::EVisibilityFlags OvCore::ECS::Components::CMaterialRenderer::GetVisibilityFlags() const
{
	return m_visibilityFlags;
}

bool OvCore::ECS::Components::CMaterialRenderer::HasVisibilityFlags(OvCore::Rendering::EVisibilityFlags p_flags) const
{
	return OvCore::Rendering::SatisfiesVisibility(m_visibilityFlags, p_flags);
}

void OvCore::ECS::Components::CMaterialRenderer::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	tinyxml2::XMLNode* materialsNode = p_doc.NewElement("materials");
	p_node->InsertEndChild(materialsNode);

	auto modelRenderer = owner.GetComponent<CModelRenderer>();
	uint8_t elementsToSerialize = modelRenderer->GetModel() ? (uint8_t)std::min(modelRenderer->GetModel()->GetMaterialNames().size(), (size_t)kMaxMaterialCount) : 0;

	for (uint8_t i = 0; i < elementsToSerialize; ++i)
	{
		OvCore::Helpers::Serializer::SerializeMaterial(p_doc, materialsNode, "material", m_materials[i]);
	}

	OvCore::Helpers::Serializer::SerializeUint32(p_doc, p_node, "visibility_flags", reinterpret_cast<uint32_t&>(m_visibilityFlags));
}

void OvCore::ECS::Components::CMaterialRenderer::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	tinyxml2::XMLNode* materialsRoot = p_node->FirstChildElement("materials");
	if (materialsRoot)
	{
		tinyxml2::XMLElement* currentMaterial = materialsRoot->FirstChildElement("material");

		uint8_t materialIndex = 0;

		while (currentMaterial)
		{
			if (auto material = Global::ServiceLocator::Get<ResourceManagement::MaterialManager>()[currentMaterial->GetText()])
				m_materials[materialIndex] = material;

			currentMaterial = currentMaterial->NextSiblingElement("material");
			++materialIndex;
		}
	}

	UpdateMaterialList();

	OvCore::Helpers::Serializer::DeserializeUint32(p_doc, p_node, "visibility_flags", reinterpret_cast<uint32_t&>(m_visibilityFlags));
}

std::array<OvUI::Widgets::AWidget*, 2> CustomMaterialDrawer(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data)
{
	const size_t before = p_root.GetWidgets().size();
	OvCore::Helpers::GUIDrawer::DrawMaterial(p_root, p_name, p_data, nullptr);
	auto& widgets = p_root.GetWidgets();
	// DrawMaterial adds exactly 2 widgets: [before]=TextColored title, [before+1]=AssetField
	return { widgets[before].first, widgets[before + 1].first };
}

void OvCore::ECS::Components::CMaterialRenderer::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	using namespace OvCore::Helpers;
	using enum Rendering::EVisibilityFlags;

	m_inspectorRoot = &p_root;

	for (auto& materialField : m_materialFields)
	{
		materialField.fill(nullptr);
	}

	auto drawVisibilityToggle = [this, &p_root](const std::string& p_flagName, Rendering::EVisibilityFlags p_flag) {
		GUIDrawer::DrawBoolean(
			p_root,
			std::format("Visibility: {}", p_flagName),
			[this, p_flag] { return IsFlagSet(m_visibilityFlags, p_flag); },
			[this, p_flag](bool p_value) {
				if (p_value) m_visibilityFlags |= p_flag;
				else m_visibilityFlags &= ~p_flag;
			}
		);
	};

	drawVisibilityToggle("Geometry", GEOMETRY);
	drawVisibilityToggle("Reflection", REFLECTION);
	drawVisibilityToggle("Shadow", SHADOW);

	p_root.CreateWidget<OvUI::Widgets::Visual::Separator>();

	uint8_t materialCount = 0;

	if (auto modelRenderer = owner.GetComponent<CModelRenderer>(); modelRenderer && modelRenderer->GetModel())
	{
		materialCount = static_cast<uint8_t>(std::min(modelRenderer->GetModel()->GetMaterialNames().size(), static_cast<size_t>(kMaxMaterialCount)));
	}

	for (uint8_t i = 0; i < materialCount; ++i)
	{
		m_materialFields[i] = CustomMaterialDrawer(p_root, "Material", m_materials[i]);
	}

	UpdateMaterialList();
}

void OvCore::ECS::Components::CMaterialRenderer::UpdateMaterialList()
{
	if (auto modelRenderer = owner.GetComponent<CModelRenderer>(); modelRenderer && modelRenderer->GetModel())
	{
		uint8_t materialIndex = 0;

		for (const std::string& materialName : modelRenderer->GetModel()->GetMaterialNames())
		{
			m_materialNames[materialIndex++] = materialName;
		}

		for (uint8_t i = materialIndex; i < kMaxMaterialCount; ++i)
			m_materialNames[i] = "";
	}
	else
	{
		m_materialNames.fill("");
	}

	for (uint8_t i = 0; i < m_materialFields.size(); ++i)
	{
		// If the slot became active but has no widget yet, create it now.
		if (!m_materialFields[i][0] && !m_materialNames[i].empty() && m_inspectorRoot)
			m_materialFields[i] = CustomMaterialDrawer(*m_inspectorRoot, "Material", m_materials[i]);

		if (m_materialFields[i][0])
		{
			const bool enabled = !m_materialNames[i].empty();
			m_materialFields[i][0]->enabled = enabled;
			m_materialFields[i][1]->enabled = enabled;
			static_cast<OvUI::Widgets::Texts::Text*>(m_materialFields[i][0])->content =
				std::format("Material [{}]: <{}>", i, m_materialNames[i]);
			static_cast<OvUI::Widgets::InputFields::AssetField*>(m_materialFields[i][1])->content =
				m_materials[i] ? m_materials[i]->path : std::string{};
		}
	}
}

void OvCore::ECS::Components::CMaterialRenderer::FillWithEmbeddedMaterials(bool p_overwriteExisting, OvCore::Resources::Material* p_fallbackMaterial)
{
	auto* modelRenderer = owner.GetComponent<CModelRenderer>();
	if (!modelRenderer)
	{
		return;
	}

	const auto* model = modelRenderer->GetModel();
	if (!model)
	{
		return;
	}

	auto& materialManager = Global::ServiceLocator::Get<ResourceManagement::MaterialManager>();

	const uint8_t materialCount = static_cast<uint8_t>(std::min(
		model->GetMaterialNames().size(),
		static_cast<size_t>(kMaxMaterialCount)
	));
	const size_t embeddedMaterialCount = model->GetEmbeddedMaterials().size();

	for (uint8_t i = 0; i < materialCount; ++i)
	{
		auto* currentMaterial = GetMaterialAtIndex(i);
		const bool shouldOverride = p_overwriteExisting || !currentMaterial;
		if (!shouldOverride)
		{
			continue;
		}

		if (i >= embeddedMaterialCount)
		{
			if (p_fallbackMaterial)
			{
				SetMaterialAtIndex(i, *p_fallbackMaterial);
			}
			else if (p_overwriteExisting)
			{
				RemoveMaterialAtIndex(i);
			}
			continue;
		}

		const auto embeddedMaterialPath = OvRendering::Resources::Parsers::MakeEmbeddedMaterialPath(model->path, i);
		if (auto* embeddedMaterial = materialManager.GetResource(embeddedMaterialPath))
		{
			SetMaterialAtIndex(i, *embeddedMaterial);
		}
		else if (p_fallbackMaterial)
		{
			SetMaterialAtIndex(i, *p_fallbackMaterial);
		}
		else if (p_overwriteExisting)
		{
			RemoveMaterialAtIndex(i);
		}
	}

	if (p_overwriteExisting)
	{
		for (uint8_t i = materialCount; i < kMaxMaterialCount; ++i)
		{
			RemoveMaterialAtIndex(i);
		}
	}
}

void OvCore::ECS::Components::CMaterialRenderer::SetUserMatrixElement(uint32_t p_row, uint32_t p_column, float p_value)
{
	if (p_row < 4 && p_column < 4)
		m_userMatrix.data[4 * p_row + p_column] = p_value;
}

float OvCore::ECS::Components::CMaterialRenderer::GetUserMatrixElement(uint32_t p_row, uint32_t p_column) const
{
	if (p_row < 4 && p_column < 4)
		return m_userMatrix.data[4 * p_row + p_column];
	else
		return 0.0f;
}
