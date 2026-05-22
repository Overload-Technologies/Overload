/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <functional>

#include <tinyxml2.h>

#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>

#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr float kMinimumReferenceResolutionAxis = 1.0f;
	constexpr float kMinimumScaleFactor = 0.0001f;
	constexpr float kMinimumPixelsPerUnit = 0.0001f;

	float ClampFinite(float p_value, float p_min)
	{
		return std::isfinite(p_value) ? std::max(p_value, p_min) : p_min;
	}

	OvCore::ECS::Components::UI::CCanvas::EScalerMode ToScalerMode(int p_value)
	{
		using EScalerMode = OvCore::ECS::Components::UI::CCanvas::EScalerMode;

		switch (p_value)
		{
		case static_cast<int>(EScalerMode::SCALE_WITH_SCREEN_SIZE):
			return EScalerMode::SCALE_WITH_SCREEN_SIZE;
		case static_cast<int>(EScalerMode::CONSTANT_PIXEL_SIZE):
		default:
			return EScalerMode::CONSTANT_PIXEL_SIZE;
		}
	}
}

OvCore::ECS::Components::UI::CCanvas::CCanvas(ECS::Actor& p_owner) :
AComponent(p_owner)
{
}

std::string OvCore::ECS::Components::UI::CCanvas::GetName()
{
	return "Canvas";
}

std::string OvCore::ECS::Components::UI::CCanvas::GetTypeName()
{
	return std::string{ComponentTraits<CCanvas>::Name};
}

void OvCore::ECS::Components::UI::CCanvas::SetReferenceResolution(const OvMaths::FVector2& p_referenceResolution)
{
	m_referenceResolution.x = ClampFinite(p_referenceResolution.x, kMinimumReferenceResolutionAxis);
	m_referenceResolution.y = ClampFinite(p_referenceResolution.y, kMinimumReferenceResolutionAxis);
}

const OvMaths::FVector2& OvCore::ECS::Components::UI::CCanvas::GetReferenceResolution() const
{
	return m_referenceResolution;
}

void OvCore::ECS::Components::UI::CCanvas::SetScaleFactor(float p_scaleFactor)
{
	m_scaleFactor = ClampFinite(p_scaleFactor, kMinimumScaleFactor);
}

float OvCore::ECS::Components::UI::CCanvas::GetScaleFactor() const
{
	return m_scaleFactor;
}

void OvCore::ECS::Components::UI::CCanvas::SetPixelsPerUnit(float p_pixelsPerUnit)
{
	m_pixelsPerUnit = ClampFinite(p_pixelsPerUnit, kMinimumPixelsPerUnit);
}

float OvCore::ECS::Components::UI::CCanvas::GetPixelsPerUnit() const
{
	return m_pixelsPerUnit;
}

void OvCore::ECS::Components::UI::CCanvas::SetScalerMode(EScalerMode p_scalerMode)
{
	m_scalerMode = ToScalerMode(static_cast<int>(p_scalerMode));
}

OvCore::ECS::Components::UI::CCanvas::EScalerMode OvCore::ECS::Components::UI::CCanvas::GetScalerMode() const
{
	return m_scalerMode;
}

void OvCore::ECS::Components::UI::CCanvas::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	Helpers::Serializer::SerializeVec2(p_doc, p_node, "reference_resolution", m_referenceResolution);
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "scale_factor", m_scaleFactor);
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "pixels_per_unit", m_pixelsPerUnit);
	Helpers::Serializer::SerializeInt(p_doc, p_node, "scaler_mode", static_cast<int>(m_scalerMode));
}

void OvCore::ECS::Components::UI::CCanvas::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	if (p_node->FirstChildElement("reference_resolution"))
	{
		auto referenceResolution = m_referenceResolution;
		Helpers::Serializer::DeserializeVec2(p_doc, p_node, "reference_resolution", referenceResolution);
		SetReferenceResolution(referenceResolution);
	}

	if (p_node->FirstChildElement("scale_factor"))
	{
		auto scaleFactor = m_scaleFactor;
		Helpers::Serializer::DeserializeFloat(p_doc, p_node, "scale_factor", scaleFactor);
		SetScaleFactor(scaleFactor);
	}

	if (p_node->FirstChildElement("pixels_per_unit"))
	{
		auto pixelsPerUnit = m_pixelsPerUnit;
		Helpers::Serializer::DeserializeFloat(p_doc, p_node, "pixels_per_unit", pixelsPerUnit);
		SetPixelsPerUnit(pixelsPerUnit);
	}

	if (p_node->FirstChildElement("scaler_mode"))
	{
		auto scalerMode = static_cast<int>(m_scalerMode);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "scaler_mode", scalerMode);
		SetScalerMode(ToScalerMode(scalerMode));
	}
}

void OvCore::ECS::Components::UI::CCanvas::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	Helpers::GUIDrawer::DrawVec2(
		p_root,
		"Reference Resolution",
		[this]() { return GetReferenceResolution(); },
		[this](OvMaths::FVector2 p_value) { SetReferenceResolution(p_value); },
		1.0f,
		kMinimumReferenceResolutionAxis
	);

	Helpers::GUIDrawer::DrawScalar<float>(
		p_root,
		"Scale Factor",
		std::bind(&CCanvas::GetScaleFactor, this),
		std::bind(&CCanvas::SetScaleFactor, this, std::placeholders::_1),
		0.01f,
		kMinimumScaleFactor
	);

	Helpers::GUIDrawer::DrawScalar<float>(
		p_root,
		"Pixels Per Unit",
		std::bind(&CCanvas::GetPixelsPerUnit, this),
		std::bind(&CCanvas::SetPixelsPerUnit, this, std::placeholders::_1),
		1.0f,
		kMinimumPixelsPerUnit
	);

	Helpers::GUIDrawer::CreateTitle(p_root, "Scaler Mode");
	auto& scalerMode = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetScalerMode()));
	scalerMode.choices.emplace(static_cast<int>(EScalerMode::CONSTANT_PIXEL_SIZE), "Constant Pixel Size");
	scalerMode.choices.emplace(static_cast<int>(EScalerMode::SCALE_WITH_SCREEN_SIZE), "Scale With Screen Size");
	scalerMode.ValueChangedEvent += [this](int p_choice)
	{
		SetScalerMode(ToScalerMode(p_choice));
	};
}
