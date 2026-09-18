/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/InputHelpers.h>
#include <OvWindowing/Inputs/InputManager.h>
#include <OvWindowing/Window.h>

namespace
{
	OvCore::Helpers::InputHelpers::MousePositionProviderCallback __MOUSE_POSITION_PROVIDER;
	OvCore::Helpers::InputHelpers::ViewportSizeProviderCallback __VIEWPORT_SIZE_PROVIDER;
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

void OvCore::Helpers::InputHelpers::SetViewportSizeProvider(ViewportSizeProviderCallback p_provider)
{
	__VIEWPORT_SIZE_PROVIDER = std::move(p_provider);
}

OvMaths::FVector2 OvCore::Helpers::InputHelpers::GetViewportSize()
{
	if (__VIEWPORT_SIZE_PROVIDER)
	{
		return __VIEWPORT_SIZE_PROVIDER();
	}

	const auto [width, height] = OVSERVICE(OvWindowing::Window).GetSize();
	return { static_cast<float>(width), static_cast<float>(height) };
}
