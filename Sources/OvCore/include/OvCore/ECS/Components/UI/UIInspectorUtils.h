/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Internal/WidgetContainer.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI::UIInspectorUtils
{
	/**
	* Draws whether the given actor has a canvas on one of its parents, which is required to render its user interface components
	* @param p_root
	* @param p_owner
	*/
	void DrawCanvasRequirement(OvUI::Internal::WidgetContainer& p_root, const ECS::Actor& p_owner);
}
