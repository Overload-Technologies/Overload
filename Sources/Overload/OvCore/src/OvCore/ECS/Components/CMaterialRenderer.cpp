/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>
#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/MaterialManager.h>

#include <OvTools/Utils/PathParser.h>

#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Buttons/ButtonSmall.h>
#include <OvUI/Plugins/DDTarget.h>
#include <OvUI/Widgets/InputFields/InputInt.h>
#include <OvUI/Widgets/Layout/Dummy.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/TextColored.h>
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

std::array<OvUI::Widgets::AWidget*, 3> CustomMaterialDrawer(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data)
{
	using namespace OvCore::Helpers;

	std::array<OvUI::Widgets::AWidget*, 3> widgets;

	widgets[0] = &p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>(p_name, GUIDrawer::TitleColor);

	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
	auto & rightSide = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();

	auto& widget = rightSide.CreateWidget<OvUI::Widgets::Texts::Text>(displayedText);

	widgets[1] = &widget;

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [&widget, &p_data](auto p_receivedData)
	{
		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MATERIAL)
		{
			if (auto resource = OVSERVICE(OvCore::ResourceManagement::MaterialManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widget.content = p_receivedData.first;
			}
		}
	};

	widget.lineBreak = false;

	auto & resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
	resetButton.idleBackgroundColor = GUIDrawer::ClearButtonColor;
	resetButton.ClickedEvent += [&widget, &p_data]
	{
		p_data = nullptr;
		widget.content = "Empty";
	};

	widgets[2] = &resetButton;

	return widgets;
}

void OvCore::ECS::Components::CMaterialRenderer::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	using namespace OvCore::Helpers;
	using enum Rendering::EVisibilityFlags;

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
	p_root.CreateWidget<OvUI::Widgets::Layout::Dummy>(); // Necessary to fill the "value" column

	for (uint8_t i = 0; i < m_materials.size(); ++i)
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

	for (uint8_t i = 0; i < m_materialFields.size(); ++i)
	{
		if (m_materialFields[i][0])
		{
			bool enabled = !m_materialNames[i].empty();
			m_materialFields[i][0]->enabled = enabled;
			m_materialFields[i][1]->enabled = enabled;
			m_materialFields[i][2]->enabled = enabled;
			const auto formattedName = std::format("Material [{}]: <{}>", i, m_materialNames[i]);
			reinterpret_cast<OvUI::Widgets::Texts::Text*>(m_materialFields[i][0]) ->content = formattedName;
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
