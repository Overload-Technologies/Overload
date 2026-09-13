/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/InputHelpers.h>
#include <OvWindowing/Inputs/InputManager.h>

namespace
{
	OvCore::Helpers::InputHelpers::MousePositionProviderCallback __MOUSE_POSITION_PROVIDER;
}

void OvCore::Helpers::InputHelpers::SetMousePositionProvider(MousePositionProviderCallback p_provider)
{
	__MOUSE_POSITION_PROVIDER = std::move(p_provider);
}

OvMaths::FVector2 OvCore::Helpers::InputHelpers::GetMousePosition()
{
	if (__MOUSE_POSITION_PROVIDER)
	{
		return __MOUSE_POSITION_PROVIDER();
	}

	const auto [mouseX, mouseY] = OVSERVICE(OvWindowing::Inputs::InputManager).GetMousePosition();
	return { static_cast<float>(mouseX), static_cast<float>(mouseY) };
}
