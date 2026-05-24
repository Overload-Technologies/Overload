/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/Rendering/UIRenderingUtils.h>

namespace
{
	constexpr float kMinimumCanvasScale = 0.0001f;

	float ClampFinite(float p_value, float p_min)
	{
		return std::isfinite(p_value) ? std::max(p_value, p_min) : p_min;
	}
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::ClampCanvasSize(const OvMaths::FVector2& p_canvasSize)
{
	return {
		std::max(p_canvasSize.x, 1.0f),
		std::max(p_canvasSize.y, 1.0f)
	};
}

float OvCore::Rendering::UIRenderingUtils::GetCanvasScale(
	const OvCore::ECS::Components::UI::CCanvas& p_canvas,
	const OvMaths::FVector2& p_renderSize
)
{
	const auto renderSize = ClampCanvasSize(p_renderSize);
	const auto referenceResolution = ClampCanvasSize(p_canvas.GetReferenceResolution());
	const auto scaleFactor = ClampFinite(p_canvas.GetScaleFactor(), kMinimumCanvasScale);

	if (p_canvas.GetScalerMode() == OvCore::ECS::Components::UI::CCanvas::EScalerMode::CONSTANT_PIXEL_SIZE)
	{
		return scaleFactor;
	}

	const float widthScale = renderSize.x / referenceResolution.x;
	const float heightScale = renderSize.y / referenceResolution.y;
	float screenScale = 1.0f;

	switch (p_canvas.GetScreenMatchMode())
	{
	case OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::EXPAND:
		screenScale = std::min(widthScale, heightScale);
		break;
	case OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::SHRINK:
		screenScale = std::max(widthScale, heightScale);
		break;
	case OvCore::ECS::Components::UI::CCanvas::EScreenMatchMode::MATCH_WIDTH_OR_HEIGHT:
	default:
	{
		const auto match = std::clamp(p_canvas.GetMatchWidthOrHeight(), 0.0f, 1.0f);
		const auto logWidth = std::log2(std::max(widthScale, kMinimumCanvasScale));
		const auto logHeight = std::log2(std::max(heightScale, kMinimumCanvasScale));
		screenScale = std::pow(2.0f, logWidth + (logHeight - logWidth) * match);
		break;
	}
	}

	return ClampFinite(screenScale * scaleFactor, kMinimumCanvasScale);
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::GetCanvasSize(
	const OvCore::ECS::Components::UI::CCanvas& p_canvas,
	const OvMaths::FVector2& p_renderSize
)
{
	const auto renderSize = ClampCanvasSize(p_renderSize);
	const auto canvasScale = GetCanvasScale(p_canvas, renderSize);
	return ClampCanvasSize(renderSize / canvasScale);
}

const OvCore::ECS::Components::UI::CCanvas* OvCore::Rendering::UIRenderingUtils::FindCanvas(const OvCore::ECS::Actor& p_owner)
{
	const auto* current = &p_owner;

	while (current)
	{
		if (const auto* canvas = current->GetComponent<OvCore::ECS::Components::UI::CCanvas>(); canvas)
		{
			return canvas;
		}

		current = current->GetParent();
	}

	return nullptr;
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::GetCanvasSize(
	const OvCore::ECS::Actor& p_owner,
	const OvMaths::FVector2& p_renderSize
)
{
	if (const auto* canvas = FindCanvas(p_owner))
	{
		return GetCanvasSize(*canvas, p_renderSize);
	}

	return ClampCanvasSize(p_renderSize);
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::GetLayoutOffset(const OvCore::ECS::Actor& p_owner)
{
	OvMaths::FVector2 result = OvMaths::FVector2::Zero;
	const auto* child = &p_owner;

	while (const auto* parent = child->GetParent())
	{
		if (const auto* layout = parent->GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>())
		{
			result += layout->GetChildOffset(*child);
		}

		child = parent;
	}

	return result;
}

float OvCore::Rendering::UIRenderingUtils::GetUIWorldScale(
	const OvCore::ECS::Components::UI::CCanvas& p_canvas,
	bool p_screenSpace
)
{
	if (p_screenSpace)
	{
		return 1.0f;
	}

	return 1.0f / p_canvas.GetPixelsPerUnit();
}
