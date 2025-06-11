/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

namespace OvEditor::Settings
{
	enum class EFontSize
	{
		SMALL = 0,
		MEDIUM = 1,
		BIG = 2,
		DEFAULT = MEDIUM
	};

	constexpr std::string_view GetFontID(EFontSize p_size)
	{
		switch (p_size)
		{
			case EFontSize::SMALL: return "Small";
			case EFontSize::MEDIUM: return "Medium";
			case EFontSize::BIG: return "Big";
		}

		return {};
	}
}
