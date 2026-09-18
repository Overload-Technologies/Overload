/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>

#include <OvMaths/FVector2.h>

namespace OvCore::Helpers
{
	/**
	* Input helpers shared between the editor and the game
	*/
	class InputHelpers
	{
	public:
		using MousePositionProviderCallback = std::function<OvMaths::FVector2()>;
		using ViewportSizeProviderCallback = std::function<OvMaths::FVector2()>;

		/**
		* Defines the callback returning the mouse position relative to the area where the game is rendered
		* @param p_provider
		*/
		static void SetMousePositionProvider(MousePositionProviderCallback p_provider);

		/**
		* Returns the mouse position relative to the area where the game is rendered
		* @note Without provider, the position is relative to the window, which is the rendered area of a built game
		*/
		static OvMaths::FVector2 GetMousePosition();

		/**
		* Defines the callback returning the size of the area where the game is rendered
		* @param p_provider
		*/
		static void SetViewportSizeProvider(ViewportSizeProviderCallback p_provider);

		/**
		* Returns the size of the area where the game is rendered
		* @note Without provider, the size is the window size, which is the rendered area of a built game
		*/
		static OvMaths::FVector2 GetViewportSize();
	};
}
