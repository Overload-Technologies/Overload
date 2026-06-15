/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <functional>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CTransform.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/ECS/Components/UI/UITransformResolver.h>
#include <OvCore/Rendering/UIRenderingUtils.h>
#include <OvMaths/FQuaternion.h>
#include <OvMaths/FVector4.h>

namespace
{
	constexpr float kDegreesToRadians = 3.14159265359f / 180.0f;
	constexpr float kMinimumCanvasScale = 0.0001f;

	float ClampFinite(float p_value, float p_min)
	{
		return std::isfinite(p_value) ? std::max(p_value, p_min) : p_min;
	}

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

	OvMaths::FMatrix4 CalculateUnscaledModelMatrix(const OvCore::ECS::Actor& p_actor)
	{
		return
			OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition()) *
			OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
	}

	OvMaths::FVector2 GetLocalAnchoredPosition(
		const OvCore::ECS::Components::CTransform& p_transform,
		const OvMaths::FVector2& p_parentSize,
		const OvMaths::FVector2& p_layoutOffset,
		bool p_drivenByLayout
	)
	{
		if (p_drivenByLayout)
		{
			return p_layoutOffset;
		}

		const auto anchorPreset = p_transform.GetUIAnchorPreset();
		const auto anchorRatio = OvCore::ECS::Components::UI::UITransformResolver::GetAnchorRatio(anchorPreset);
		const OvMaths::FVector2 anchorOffset = {
			KeepFinite(p_parentSize.x, 0.0f) * anchorRatio.x,
			KeepFinite(p_parentSize.y, 0.0f) * anchorRatio.y
		};
		const float positionX = OvCore::ECS::Components::UI::UITransformResolver::IsHorizontalPositionEditable(anchorPreset) ?
			p_transform.GetUIPosition().x :
			0.0f;
		const float positionY = OvCore::ECS::Components::UI::UITransformResolver::IsVerticalPositionEditable(anchorPreset) ?
			p_transform.GetUIPosition().y :
			0.0f;

		return {
			anchorOffset.x + p_layoutOffset.x + positionX,
			anchorOffset.y + p_layoutOffset.y + positionY
		};
	}

	OvMaths::FMatrix4 CreateUIElementFrameMatrix(
		const OvCore::ECS::Components::CTransform& p_transform,
		const OvMaths::FVector2& p_parentSize,
		const OvMaths::FVector2& p_layoutOffset,
		const OvMaths::FVector2& p_effectiveSize,
		bool p_drivenByLayout
	)
	{
		const auto position = GetLocalAnchoredPosition(
			p_transform,
			p_parentSize,
			p_layoutOffset,
			p_drivenByLayout
		);
		const auto scale = p_transform.GetUIScale();
		const auto halfSize = p_effectiveSize * 0.5f;
		const auto& pivot = p_transform.GetUIPivot();
		const OvMaths::FVector2 pivotOffset = {
			-pivot.x * halfSize.x,
			pivot.y * halfSize.y
		};

		return
			OvMaths::FMatrix4::Translation({ position.x, position.y, 0.0f }) *
			OvMaths::FMatrix4::RotationOnAxisZ(p_transform.GetUIRotation() * kDegreesToRadians) *
			OvMaths::FMatrix4::Scaling({ scale.x, scale.y, 1.0f }) *
			OvMaths::FMatrix4::Translation({ pivotOffset.x, pivotOffset.y, 0.0f });
	}

	OvMaths::FMatrix4 ApplyElementSizeScaling(
		OvMaths::FMatrix4 p_matrix,
		const OvMaths::FVector2& p_elementSize,
		const OvMaths::FVector2& p_effectiveSize
	)
	{
		if (p_elementSize.x > 0.0f || p_elementSize.y > 0.0f)
		{
			p_matrix = p_matrix * OvMaths::FMatrix4::Scaling({
				p_elementSize.x > 0.0f ? p_effectiveSize.x / p_elementSize.x : 1.0f,
				p_elementSize.y > 0.0f ? p_effectiveSize.y / p_elementSize.y : 1.0f,
				1.0f
			});
		}

		return p_matrix;
	}
}

OvCore::Rendering::UIRenderingUtils::UIFrameResolver::UIFrameResolver(
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace
) :
m_renderSize(ClampCanvasSize(p_renderSize)),
m_screenSpace(p_screenSpace)
{
}

const OvMaths::FVector2& OvCore::Rendering::UIRenderingUtils::UIFrameResolver::GetRenderSize() const
{
	return m_renderSize;
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::IsScreenSpace() const
{
	return m_screenSpace;
}

OvMaths::FMatrix4 OvCore::Rendering::UIRenderingUtils::UIFrameResolver::CreateProjectionMatrix(
	float p_near,
	float p_far
) const
{
	return CreateUIProjectionMatrix(m_renderSize, p_near, p_far);
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::UIFrameResolver::GetElementSize(const OvCore::ECS::Actor& p_actor) const
{
	if (const auto it = m_elementSizeCache.find(&p_actor); it != m_elementSizeCache.end())
	{
		return it->second;
	}

	const auto elementSize = UIRenderingUtils::GetElementSize(p_actor, m_renderSize);
	m_elementSizeCache.emplace(&p_actor, elementSize);
	return elementSize;
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ResolveCanvas(
	const OvCore::ECS::Actor& p_actor,
	ResolvedUICanvas& p_outCanvas
) const
{
	if (const auto it = m_canvasCache.find(&p_actor); it != m_canvasCache.end())
	{
		if (!it->second)
		{
			return false;
		}

		p_outCanvas = it->second.value();
		return true;
	}

	ResolvedUICanvas resolvedCanvas;
	if (!ResolveCanvasUncached(p_actor, resolvedCanvas))
	{
		m_canvasCache.emplace(&p_actor, std::nullopt);
		return false;
	}

	m_canvasCache.emplace(&p_actor, resolvedCanvas);
	p_outCanvas = resolvedCanvas;
	return true;
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ResolveElement(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_elementSize,
	ResolvedUIElement& p_outElement
) const
{
	const ElementKey key{
		.actor = &p_actor,
		.width = p_elementSize.x,
		.height = p_elementSize.y
	};

	if (const auto it = m_elementCache.find(key); it != m_elementCache.end())
	{
		if (!it->second)
		{
			return false;
		}

		p_outElement = it->second.value();
		return true;
	}

	ResolvedUIElement resolvedElement;
	if (!ResolveElementUncached(p_actor, p_elementSize, resolvedElement))
	{
		m_elementCache.emplace(key, std::nullopt);
		return false;
	}

	m_elementCache.emplace(key, resolvedElement);
	p_outElement = resolvedElement;
	return true;
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ResolveElement(
	const OvCore::ECS::Actor& p_actor,
	ResolvedUIElement& p_outElement
) const
{
	return ResolveElement(p_actor, GetElementSize(p_actor), p_outElement);
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ElementKey::operator==(const ElementKey& p_other) const
{
	return actor == p_other.actor && width == p_other.width && height == p_other.height;
}

std::size_t OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ElementKeyHash::operator()(const ElementKey& p_key) const
{
	const auto actorHash = std::hash<const OvCore::ECS::Actor*>{}(p_key.actor);
	const auto widthHash = std::hash<float>{}(p_key.width);
	const auto heightHash = std::hash<float>{}(p_key.height);
	return actorHash ^ (widthHash << 1) ^ (heightHash << 2);
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ResolveCanvasUncached(
	const OvCore::ECS::Actor& p_actor,
	ResolvedUICanvas& p_outCanvas
) const
{
	const auto* canvas = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>();
	if (!canvas)
	{
		return false;
	}

	p_outCanvas.actor = &p_actor;
	p_outCanvas.canvas = canvas;
	p_outCanvas.size = UIRenderingUtils::GetCanvasSize(*canvas, m_renderSize);
	p_outCanvas.matrix = m_screenSpace ? OvMaths::FMatrix4::Identity : CalculateUnscaledModelMatrix(p_actor);
	p_outCanvas.canvasScale = UIRenderingUtils::GetCanvasScale(*canvas, m_renderSize);
	p_outCanvas.worldScale = UIRenderingUtils::GetUIWorldScale(*canvas, m_screenSpace);
	p_outCanvas.unitsScale = m_screenSpace ? p_outCanvas.canvasScale : p_outCanvas.canvasScale * p_outCanvas.worldScale;
	p_outCanvas.modelMatrix =
		p_outCanvas.matrix *
		OvMaths::FMatrix4::Scaling({ p_outCanvas.unitsScale, p_outCanvas.unitsScale, 1.0f });
	p_outCanvas.screenSpace = m_screenSpace;

	return p_outCanvas.size.x > 0.0f && p_outCanvas.size.y > 0.0f;
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::ResolveElementUncached(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_elementSize,
	ResolvedUIElement& p_outElement
) const
{
	if (!HasActiveUIData(p_actor))
	{
		return false;
	}

	const auto* canvasOwner = FindCanvasOwner(p_actor);
	if (!canvasOwner)
	{
		return false;
	}

	ResolvedUICanvas resolvedCanvas;
	if (!ResolveCanvas(*canvasOwner, resolvedCanvas))
	{
		return false;
	}

	const auto& transform = p_actor.transform;
	const auto layoutData = GetLayoutData(p_actor);
	OvMaths::FMatrix4 parentFrameMatrix = resolvedCanvas.modelMatrix;
	OvMaths::FVector2 parentSize = resolvedCanvas.size;

	const auto* parent = p_actor.GetParent();
	if (parent && parent != canvasOwner && HasActiveUIData(*parent))
	{
		ResolvedUIElement resolvedParent;
		if (!ResolveElement(*parent, resolvedParent))
		{
			return false;
		}

		parentFrameMatrix = resolvedParent.frameMatrix;
		parentSize = resolvedParent.effectiveSize;
	}

	p_outElement.actor = &p_actor;
	p_outElement.canvasActor = canvasOwner;
	p_outElement.canvas = resolvedCanvas.canvas;
	p_outElement.canvasSize = resolvedCanvas.size;
	p_outElement.layoutOffset = layoutData.offset;
	p_outElement.elementSize = p_elementSize;
	p_outElement.effectiveSize = OvCore::ECS::Components::UI::UITransformResolver::GetEffectiveSize(transform, p_elementSize);
	if (layoutData.hasDirectWidth)
	{
		p_outElement.effectiveSize.x = layoutData.directSize.x;
	}
	if (layoutData.hasDirectHeight)
	{
		p_outElement.effectiveSize.y = layoutData.directSize.y;
	}
	p_outElement.canvasMatrix = resolvedCanvas.matrix;
	const auto localFrameMatrix = CreateUIElementFrameMatrix(
		transform,
		parentSize,
		p_outElement.layoutOffset,
		p_outElement.effectiveSize,
		layoutData.drivenByLayout
	);
	p_outElement.localMatrix = ApplyElementSizeScaling(
		localFrameMatrix,
		p_elementSize,
		p_outElement.effectiveSize
	);
	p_outElement.canvasScale = resolvedCanvas.canvasScale;
	p_outElement.worldScale = resolvedCanvas.worldScale;
	p_outElement.unitsScale = resolvedCanvas.unitsScale;
	p_outElement.frameMatrix = parentFrameMatrix * localFrameMatrix;
	p_outElement.modelMatrix = parentFrameMatrix * p_outElement.localMatrix;
	p_outElement.screenSpace = m_screenSpace;

	return true;
}

bool OvCore::Rendering::UIRenderingUtils::UIFrameResolver::HasActiveUIData(const OvCore::ECS::Actor& p_actor) const
{
	if (const auto it = m_activeUIDataCache.find(&p_actor); it != m_activeUIDataCache.end())
	{
		return it->second;
	}

	const bool hasActiveUIData = OvCore::ECS::Components::UI::UITransformResolver::HasActiveUIData(p_actor);
	m_activeUIDataCache.emplace(&p_actor, hasActiveUIData);
	return hasActiveUIData;
}

const OvCore::ECS::Actor* OvCore::Rendering::UIRenderingUtils::UIFrameResolver::FindCanvasOwner(const OvCore::ECS::Actor& p_actor) const
{
	if (const auto it = m_canvasOwnerCache.find(&p_actor); it != m_canvasOwnerCache.end())
	{
		return it->second;
	}

	const auto* canvasOwner = UIRenderingUtils::FindCanvasOwner(p_actor);
	m_canvasOwnerCache.emplace(&p_actor, canvasOwner);
	return canvasOwner;
}

OvCore::Rendering::UIRenderingUtils::UIFrameResolver::CachedLayoutData OvCore::Rendering::UIRenderingUtils::UIFrameResolver::GetLayoutData(const OvCore::ECS::Actor& p_actor) const
{
	if (const auto it = m_layoutDataCache.find(&p_actor); it != m_layoutDataCache.end())
	{
		return it->second;
	}

	CachedLayoutData cachedLayoutData;
	const auto* parent = p_actor.GetParent();
	if (parent)
	{
		if (const auto* layout = parent->GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>())
		{
			cachedLayoutData.drivenByLayout = true;

			if (const auto childLayout = layout->GetChildLayout(p_actor); childLayout && childLayout->valid)
			{
				cachedLayoutData.offset = childLayout->offset;

				if (childLayout->hasDirectWidth && childLayout->size.x > 0.0f)
				{
					cachedLayoutData.directSize.x = childLayout->size.x;
					cachedLayoutData.hasDirectWidth = true;
				}

				if (childLayout->hasDirectHeight && childLayout->size.y > 0.0f)
				{
					cachedLayoutData.directSize.y = childLayout->size.y;
					cachedLayoutData.hasDirectHeight = true;
				}
			}
		}
	}

	m_layoutDataCache.emplace(&p_actor, cachedLayoutData);
	return cachedLayoutData;
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
	return OvCore::ECS::Components::UI::UITransformResolver::FindCanvasOwner(p_owner);
}

const OvCore::ECS::Actor* OvCore::Rendering::UIRenderingUtils::FindCanvasOwner(const OvCore::ECS::Actor& p_owner)
{
	return OvCore::ECS::Components::UI::UITransformResolver::FindCanvasOwner(p_owner);
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
		return image->GetIntrinsicSize();
	}

	if (const auto* text = p_owner.GetComponent<OvCore::ECS::Components::UI::CText>())
	{
		return text->GetSize();
	}

	if (const auto* layout = p_owner.GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>())
	{
		return layout->GetComputedSize();
	}

	if (const auto* canvas = p_owner.GetComponent<OvCore::ECS::Components::UI::CCanvas>())
	{
		return GetCanvasSize(*canvas, p_renderSize);
	}

	return p_owner.transform.GetUISize();
}

OvMaths::FVector2 OvCore::Rendering::UIRenderingUtils::GetLayoutOffset(const OvCore::ECS::Actor& p_owner)
{
	return OvCore::ECS::Components::UI::UITransformResolver::ResolveLayoutData(p_owner).offset;
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

OvMaths::FVector3 OvCore::Rendering::UIRenderingUtils::TransformUIElementPivot(const ResolvedUIElement& p_element)
{
	if (!p_element.actor)
	{
		return TransformUIPoint(p_element.modelMatrix, OvMaths::FVector2::Zero);
	}

	const auto& pivot = p_element.actor->transform.GetUIPivot();
	const OvMaths::FVector2 referenceSize = {
		p_element.elementSize.x > 0.0f ? p_element.elementSize.x : p_element.effectiveSize.x,
		p_element.elementSize.y > 0.0f ? p_element.elementSize.y : p_element.effectiveSize.y
	};

	return TransformUIPoint(
		p_element.modelMatrix,
		{
			pivot.x * referenceSize.x * 0.5f,
			-pivot.y * referenceSize.y * 0.5f
		}
	);
}

bool OvCore::Rendering::UIRenderingUtils::ResolveUICanvas(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace,
	ResolvedUICanvas& p_outCanvas
)
{
	return UIFrameResolver(p_renderSize, p_screenSpace).ResolveCanvas(p_actor, p_outCanvas);
}

bool OvCore::Rendering::UIRenderingUtils::ResolveUIElement(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace,
	const OvMaths::FVector2& p_elementSize,
	ResolvedUIElement& p_outElement
)
{
	return UIFrameResolver(p_renderSize, p_screenSpace).ResolveElement(p_actor, p_elementSize, p_outElement);
}

bool OvCore::Rendering::UIRenderingUtils::ResolveUIElement(
	const OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector2& p_renderSize,
	bool p_screenSpace,
	ResolvedUIElement& p_outElement
)
{
	return UIFrameResolver(p_renderSize, p_screenSpace).ResolveElement(p_actor, p_outElement);
}
