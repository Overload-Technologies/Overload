/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector2.h>

namespace OvCore::ECS { class Actor; }
namespace OvCore::ECS::Components::UI { class CCanvas; }

namespace OvCore::Rendering::UIRenderingUtils
{
	OvMaths::FVector2 ClampCanvasSize(const OvMaths::FVector2& p_canvasSize);

	OvMaths::FVector2 GetCanvasSize(
		const OvCore::ECS::Components::UI::CCanvas& p_canvas,
		const OvMaths::FVector2& p_renderSize
	);

	float GetCanvasScale(
		const OvCore::ECS::Components::UI::CCanvas& p_canvas,
		const OvMaths::FVector2& p_renderSize
	);

	const OvCore::ECS::Components::UI::CCanvas* FindCanvas(const OvCore::ECS::Actor& p_owner);

	OvMaths::FVector2 GetCanvasSize(
		const OvCore::ECS::Actor& p_owner,
		const OvMaths::FVector2& p_renderSize
	);

	OvMaths::FVector2 GetLayoutOffset(const OvCore::ECS::Actor& p_owner);

	float GetUIWorldScale(
		const OvCore::ECS::Components::UI::CCanvas& p_canvas,
		bool p_screenSpace
	);
}
