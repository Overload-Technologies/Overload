/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CTransform.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/Rendering/UIRenderingUtils.h>
#include <OvMaths/FQuaternion.h>
#include <OvMaths/FVector4.h>

namespace
{
	constexpr float kMinimumCanvasScale = 0.0001f;

	float ClampFinite(float p_value, float p_min)
	{
		return std::isfinite(p_value) ? std::max(p_value, p_min) : p_min;
	}

	OvMaths::FMatrix4 CalculateUnscaledModelMatrix(const OvCore::ECS::Actor& p_actor)
	{
		return
			OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition()) *
			OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
	}
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::ClampCanvasSize(const OvMaths::FVector2& p_canvasSize)
{
	return {
		std::max(p_canvasSize.x, 1.0f),
		std::max(p_canvasSize.y, 1.0f)
	};
}

OvMaths::FMatrix4 OvCore::Rendering::UIRenderingUtils::CreateUIProjectionMatrix(
	const OvMaths::FVector2& p_renderSize,
	float p_near,
	float p_far
)
{
	const auto renderSize = ClampCanvasSize(p_renderSize);
	const auto aspectRatio = renderSize.x / renderSize.y;

	return OvMaths::FMatrix4::CreateOrthographic(renderSize.y * 0.5f, aspectRatio, p_near, p_far);
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
	if (p_canvas.GetScalerMode() == OvCore::ECS::Components::UI::CCanvas::EScalerMode::CONSTANT_PIXEL_SIZE)
	{
		return ClampCanvasSize(p_canvas.GetReferenceResolution());
	}

	const auto renderSize = ClampCanvasSize(p_renderSize);
	const auto canvasScale = GetCanvasScale(p_canvas, renderSize);
	return ClampCanvasSize(renderSize / canvasScale);
}

const OvCore::ECS::Components::UI::CCanvas* OvCore::Rendering::UIRenderingUtils::FindCanvas(const OvCore::ECS::Actor& p_owner)
{
	if (const auto* canvasOwner = FindCanvasOwner(p_owner))
	{
		return canvasOwner->GetComponent<OvCore::ECS::Components::UI::CCanvas>();
	}

	return nullptr;
}

OvCore::ECS::Actor* OvCore::Rendering::UIRenderingUtils::FindCanvasOwner(OvCore::ECS::Actor& p_owner)
{
	auto* current = &p_owner;

	while (current)
	{
		if (current->GetComponent<OvCore::ECS::Components::UI::CCanvas>())
		{
			return current;
		}

		current = current->GetParent();
	}

	return nullptr;
}

const OvCore::ECS::Actor* OvCore::Rendering::UIRenderingUtils::FindCanvasOwner(const OvCore::ECS::Actor& p_owner)
{
	auto* current = &p_owner;

	while (current)
	{
		if (current->GetComponent<OvCore::ECS::Components::UI::CCanvas>())
		{
			return current;
		}

		current = current->GetParent();
	}

	return nullptr;
}

OvMaths::FMatrix4 OvCore::Rendering::UIRenderingUtils::GetCanvasMatrix(
	const OvCore::ECS::Actor& p_owner,
	bool p_screenSpace
)
{
	if (p_screenSpace)
	{
		return OvMaths::FMatrix4::Identity;
	}

	if (const auto* canvasOwner = FindCanvasOwner(p_owner))
	{
		return CalculateUnscaledModelMatrix(*canvasOwner);
	}

	return OvMaths::FMatrix4::Identity;
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

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::GetElementSize(
	const OvCore::ECS::Actor& p_owner,
	const OvMaths::FVector2& p_renderSize
)
{
	if (const auto* image = p_owner.GetComponent<OvCore::ECS::Components::UI::CImage>())
	{
		return image->GetSize();
	}

	if (const auto* text = p_owner.GetComponent<OvCore::ECS::Components::UI::CText>())
	{
		return text->GetSize();
	}

	if (const auto* canvas = p_owner.GetComponent<OvCore::ECS::Components::UI::CCanvas>())
	{
		return GetCanvasSize(*canvas, p_renderSize);
	}

	return p_owner.transform.GetUISize();
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

OvMaths::FVector3 OvCore::Rendering::UIRenderingUtils::TransformUIPoint(
	const OvMaths::FMatrix4& p_matrix,
	const OvMaths::FVector2& p_point
)
{
	const auto result = p_matrix * OvMaths::FVector4{ p_point.x, p_point.y, 0.0f, 1.0f };
	return { result.x, result.y, result.z };
}

bool OvCore::Rendering::UIRenderingUtils::ResolveUICanvas(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace,
	ResolvedUICanvas& p_outCanvas
)
{
	const auto* canvas = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>();
	if (!canvas)
	{
		return false;
	}

	p_outCanvas.actor = &p_actor;
	p_outCanvas.canvas = canvas;
	p_outCanvas.size = GetCanvasSize(*canvas, p_renderSize);
	p_outCanvas.matrix = GetCanvasMatrix(p_actor, p_screenSpace);
	p_outCanvas.canvasScale = GetCanvasScale(*canvas, p_renderSize);
	p_outCanvas.worldScale = GetUIWorldScale(*canvas, p_screenSpace);
	p_outCanvas.unitsScale = p_screenSpace ? p_outCanvas.canvasScale : p_outCanvas.canvasScale * p_outCanvas.worldScale;
	p_outCanvas.modelMatrix =
		p_outCanvas.matrix *
		OvMaths::FMatrix4::Scaling({ p_outCanvas.unitsScale, p_outCanvas.unitsScale, 1.0f });
	p_outCanvas.screenSpace = p_screenSpace;

	return p_outCanvas.size.x > 0.0f && p_outCanvas.size.y > 0.0f;
}

bool OvCore::Rendering::UIRenderingUtils::ResolveUIElement(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace,
	const OvMaths::FVector2& p_elementSize,
	ResolvedUIElement& p_outElement
)
{
	const auto& transform = p_actor.transform;
	if (!transform.HasActiveUIData())
	{
		return false;
	}

	const auto* canvasOwner = FindCanvasOwner(p_actor);
	if (!canvasOwner)
	{
		return false;
	}

	const auto* canvas = canvasOwner->GetComponent<OvCore::ECS::Components::UI::CCanvas>();
	if (!canvas)
	{
		return false;
	}

	p_outElement.actor = &p_actor;
	p_outElement.canvasActor = canvasOwner;
	p_outElement.canvas = canvas;
	p_outElement.canvasSize = GetCanvasSize(*canvas, p_renderSize);
	p_outElement.layoutOffset = GetLayoutOffset(p_actor);
	p_outElement.elementSize = p_elementSize;
	p_outElement.effectiveSize = transform.GetUIEffectiveSize(p_elementSize);
	p_outElement.canvasMatrix = GetCanvasMatrix(p_actor, p_screenSpace);
	p_outElement.localMatrix = transform.GetUIMatrix(
		p_outElement.canvasSize,
		p_outElement.layoutOffset,
		p_elementSize
	);

	if (p_elementSize.x > 0.0f && p_elementSize.y > 0.0f)
	{
		p_outElement.localMatrix = p_outElement.localMatrix * OvMaths::FMatrix4::Scaling({
			p_outElement.effectiveSize.x / p_elementSize.x,
			p_outElement.effectiveSize.y / p_elementSize.y,
			1.0f
		});
	}

	p_outElement.canvasScale = GetCanvasScale(*canvas, p_renderSize);
	p_outElement.worldScale = GetUIWorldScale(*canvas, p_screenSpace);
	p_outElement.unitsScale = p_screenSpace ? p_outElement.canvasScale : p_outElement.canvasScale * p_outElement.worldScale;
	p_outElement.modelMatrix =
		p_outElement.canvasMatrix *
		OvMaths::FMatrix4::Scaling({ p_outElement.unitsScale, p_outElement.unitsScale, 1.0f }) *
		p_outElement.localMatrix;
	p_outElement.screenSpace = p_screenSpace;

	return true;
}

bool OvCore::Rendering::UIRenderingUtils::ResolveUIElement(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace,
	ResolvedUIElement& p_outElement
)
{
	return ResolveUIElement(
		p_actor,
		p_renderSize,
		p_screenSpace,
		GetElementSize(p_actor, p_renderSize),
		p_outElement
	);
}
