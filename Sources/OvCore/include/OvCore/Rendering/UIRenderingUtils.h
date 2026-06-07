/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FMatrix4.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector3.h>

namespace OvCore::ECS { class Actor; }
namespace OvCore::ECS::Components::UI { class CCanvas; }

namespace OvCore::Rendering::UIRenderingUtils
{
	struct ResolvedUICanvas
	{
		const OvCore::ECS::Actor* actor = nullptr;
		const OvCore::ECS::Components::UI::CCanvas* canvas = nullptr;
		OvMaths::FVector2 size = OvMaths::FVector2::Zero;
		OvMaths::FMatrix4 matrix = OvMaths::FMatrix4::Identity;
		OvMaths::FMatrix4 modelMatrix = OvMaths::FMatrix4::Identity;
		float canvasScale = 1.0f;
		float worldScale = 1.0f;
		float unitsScale = 1.0f;
		bool screenSpace = false;
	};

	struct ResolvedUIElement
	{
		const OvCore::ECS::Actor* actor = nullptr;
		const OvCore::ECS::Actor* canvasActor = nullptr;
		const OvCore::ECS::Components::UI::CCanvas* canvas = nullptr;
		OvMaths::FVector2 canvasSize = OvMaths::FVector2::Zero;
		OvMaths::FVector2 layoutOffset = OvMaths::FVector2::Zero;
		OvMaths::FVector2 elementSize = OvMaths::FVector2::Zero;
		OvMaths::FVector2 effectiveSize = OvMaths::FVector2::Zero;
		OvMaths::FMatrix4 canvasMatrix = OvMaths::FMatrix4::Identity;
		OvMaths::FMatrix4 localMatrix = OvMaths::FMatrix4::Identity;
		OvMaths::FMatrix4 modelMatrix = OvMaths::FMatrix4::Identity;
		float canvasScale = 1.0f;
		float worldScale = 1.0f;
		float unitsScale = 1.0f;
		bool screenSpace = false;
	};

	OvMaths::FVector2 ClampCanvasSize(const OvMaths::FVector2& p_canvasSize);

	OvMaths::FMatrix4 CreateUIProjectionMatrix(
		const OvMaths::FVector2& p_renderSize,
		float p_near = -1.0f,
		float p_far = 1.0f
	);

	OvMaths::FVector2 GetCanvasSize(
		const OvCore::ECS::Components::UI::CCanvas& p_canvas,
		const OvMaths::FVector2& p_renderSize
	);

	float GetCanvasScale(
		const OvCore::ECS::Components::UI::CCanvas& p_canvas,
		const OvMaths::FVector2& p_renderSize
	);

	const OvCore::ECS::Components::UI::CCanvas* FindCanvas(const OvCore::ECS::Actor& p_owner);

	OvCore::ECS::Actor* FindCanvasOwner(OvCore::ECS::Actor& p_owner);
	const OvCore::ECS::Actor* FindCanvasOwner(const OvCore::ECS::Actor& p_owner);

	OvMaths::FMatrix4 GetCanvasMatrix(
		const OvCore::ECS::Actor& p_owner,
		bool p_screenSpace
	);

	OvMaths::FVector2 GetCanvasSize(
		const OvCore::ECS::Actor& p_owner,
		const OvMaths::FVector2& p_renderSize
	);

	OvMaths::FVector2 GetElementSize(
		const OvCore::ECS::Actor& p_owner,
		const OvMaths::FVector2& p_renderSize
	);

	OvMaths::FVector2 GetLayoutOffset(const OvCore::ECS::Actor& p_owner);

	float GetUIWorldScale(
		const OvCore::ECS::Components::UI::CCanvas& p_canvas,
		bool p_screenSpace
	);

	OvMaths::FVector3 TransformUIPoint(
		const OvMaths::FMatrix4& p_matrix,
		const OvMaths::FVector2& p_point
	);

	bool ResolveUICanvas(
		const OvCore::ECS::Actor& p_actor,
		const OvMaths::FVector2& p_renderSize,
		bool p_screenSpace,
		ResolvedUICanvas& p_outCanvas
	);

	bool ResolveUIElement(
		const OvCore::ECS::Actor& p_actor,
		const OvMaths::FVector2& p_renderSize,
		bool p_screenSpace,
		const OvMaths::FVector2& p_elementSize,
		ResolvedUIElement& p_outElement
	);

	bool ResolveUIElement(
		const OvCore::ECS::Actor& p_actor,
		const OvMaths::FVector2& p_renderSize,
		bool p_screenSpace,
		ResolvedUIElement& p_outElement
	);
}
