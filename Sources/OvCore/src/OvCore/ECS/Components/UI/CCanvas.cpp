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

#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr float kMinimumReferenceResolutionAxis = 1.0f;
	constexpr float kMinimumScaleFactor = 0.0001f;
	constexpr float kMinimumPixelsPerUnit = 0.0001f;
	constexpr float kMinimumMatchWidthOrHeight = 0.0f;
	constexpr float kMaximumMatchWidthOrHeight = 1.0f;

	float ClampFinite(float p_value, float p_min)
	{
		return std::isfinite(p_value) ? std::max(p_value, p_min) : p_min;
	}

	float ClampFiniteNormalized(float p_value, float p_fallback)
	{
		if (!std::isfinite(p_value))
		{
			return p_fallback;
		}

		return std::clamp(p_value, kMinimumMatchWidthOrHeight, kMaximumMatchWidthOrHeight);
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

	OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode ToScreenMatchMode(int p_value)
	{
		using EScreenMatchMode = OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode;

		switch (p_value)
		{
		case static_cast<int>(EScreenMatchMode::EXPAND):
			return EScreenMatchMode::EXPAND;
		case static_cast<int>(EScreenMatchMode::SHRINK):
			return EScreenMatchMode::SHRINK;
		case static_cast<int>(EScreenMatchMode::MATCH_WIDTH_OR_HEIGHT):
		default:
			return EScreenMatchMode::MATCH_WIDTH_OR_HEIGHT;
		}
	}

	void RebuildScaleWithScreenSettings(
		OvCore::ECS::Components::UI::CCanvas& p_canvas,
		OvUI::Internal::WidgetContainer& p_root
	)
	{
		p_root.RemoveAllWidgets();

		if (p_canvas.GetScalerMode() != OvCore::ECS::Components::UI::CCanvas::EScalerMode::SCALE_WITH_SCREEN_SIZE)
		{
			return;
		}

		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, "Screen Match Mode");
		auto& screenMatchMode = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(p_canvas.GetScreenMatchMode()));
		screenMatchMode.choices.emplace(
			static_cast<int>(OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::MATCH_WIDTH_OR_HEIGHT),
			"Match Width Or Height"
		);
		screenMatchMode.choices.emplace(
			static_cast<int>(OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::EXPAND),
			"Expand"
		);
		screenMatchMode.choices.emplace(
			static_cast<int>(OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::SHRINK),
			"Shrink"
		);

		auto* canvas = &p_canvas;
		auto& screenMatchModeDispatcher = screenMatchMode.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
		screenMatchModeDispatcher.RegisterGatherer([canvas]() { return static_cast<int>(canvas->GetScreenMatchMode()); });
		screenMatchModeDispatcher.RegisterProvider([canvas](int p_choice) { canvas->SetScreenMatchMode(ToScreenMatchMode(p_choice)); });

		auto* scaleWithScreenSettings = &p_root;
		screenMatchMode.ValueChangedEvent += [canvas, scaleWithScreenSettings](int p_choice)
		{
			canvas->SetScreenMatchMode(ToScreenMatchMode(p_choice));
			RebuildScaleWithScreenSettings(*canvas, *scaleWithScreenSettings);
		};

		if (p_canvas.GetScreenMatchMode() == OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::MATCH_WIDTH_OR_HEIGHT)
		{
			OvCore::Helpers::GUIDrawer::DrawScalar<float>(
				p_root,
				"Match Width Or Height",
				[canvas]() { return canvas->GetMatchWidthOrHeight(); },
				[canvas](float p_value) { canvas->SetMatchWidthOrHeight(p_value); },
				0.01f,
				kMinimumMatchWidthOrHeight,
				kMaximumMatchWidthOrHeight
			);
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

void OvCore::ECS::Components::UI::CCanvas::SetScreenMatchMode(EScreenMatchMode p_screenMatchMode)
{
	m_screenMatchMode = ToScreenMatchMode(static_cast<int>(p_screenMatchMode));
}

OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode OvCore::ECS::Components::UI::CCanvas::GetScreenMatchMode() const
{
	return m_screenMatchMode;
}

void OvCore::ECS::Components::UI::CCanvas::SetMatchWidthOrHeight(float p_matchWidthOrHeight)
{
	m_matchWidthOrHeight = ClampFiniteNormalized(p_matchWidthOrHeight, m_matchWidthOrHeight);
}

float OvCore::ECS::Components::UI::CCanvas::GetMatchWidthOrHeight() const
{
	return m_matchWidthOrHeight;
}

void OvCore::ECS::Components::UI::CCanvas::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	Helpers::Serializer::SerializeVec2(p_doc, p_node, "reference_resolution", m_referenceResolution);
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "scale_factor", m_scaleFactor);
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "pixels_per_unit", m_pixelsPerUnit);
	Helpers::Serializer::SerializeInt(p_doc, p_node, "scaler_mode", static_cast<int>(m_scalerMode));
	Helpers::Serializer::SerializeInt(p_doc, p_node, "screen_match_mode", static_cast<int>(m_screenMatchMode));
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "match_width_or_height", m_matchWidthOrHeight);
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

	if (p_node->FirstChildElement("screen_match_mode"))
	{
		auto screenMatchMode = static_cast<int>(m_screenMatchMode);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "screen_match_mode", screenMatchMode);
		SetScreenMatchMode(ToScreenMatchMode(screenMatchMode));
	}

	if (p_node->FirstChildElement("match_width_or_height"))
	{
		auto matchWidthOrHeight = m_matchWidthOrHeight;
		Helpers::Serializer::DeserializeFloat(p_doc, p_node, "match_width_or_height", matchWidthOrHeight);
		SetMatchWidthOrHeight(matchWidthOrHeight);
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
	auto& scalerModeDispatcher = scalerMode.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	scalerModeDispatcher.RegisterGatherer([this]() { return static_cast<int>(GetScalerMode()); });
	scalerModeDispatcher.RegisterProvider([this](int p_choice) { SetScalerMode(ToScalerMode(p_choice)); });

	auto& scaleWithScreenSettings = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();
	auto* scaleWithScreenSettingsWidget = &scaleWithScreenSettings;
	auto* canvas = this;
	scalerMode.ValueChangedEvent += [canvas, scaleWithScreenSettingsWidget](int p_choice)
	{
		canvas->SetScalerMode(ToScalerMode(p_choice));
		RebuildScaleWithScreenSettings(*canvas, *scaleWithScreenSettingsWidget);
	};

	RebuildScaleWithScreenSettings(*this, scaleWithScreenSettings);
}
