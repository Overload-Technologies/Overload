/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

namespace OvWindowing::Cursor
{
	/**
	* Some cursor shapes.
	* They specify how the mouse pointer should look
	*/
	enum class ECursorShape
	{
		ARROW		= 0x00036001,
		IBEAM		= 0x00036002,
		CROSSHAIR	= 0x00036003,
		HAND		= 0x00036004,
		HRESIZE		= 0x00036005,
		VRESIZE		= 0x00036006
	};
}