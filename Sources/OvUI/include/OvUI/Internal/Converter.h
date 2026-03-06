/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <imgui.h>

#include <OvMaths/FVector2.h>
#include <OvMaths/FVector4.h>

#include <OvUI/Types/Color.h>

namespace OvUI::Internal
{
	/**
	* Handles imgui conversion to/from overload types
	*/
	class Converter
	{
	public:
		/**
		* Disabled constructor
		*/
		Converter() = delete;

		/**
		* Convert the given Color to ImVec4
		* @param p_value
		*/
		static ImVec4 ToImVec4(const Types::Color& p_value);

		/**
		* Convert the given ImVec4 to Color
		* @param p_value
		*/
		static Types::Color ToColor(const ImVec4& p_value);

		/**
		* Convert the given FVector2 to ImVec2
		* @param p_value
		*/
		static ImVec2 ToImVec2(const OvMaths::FVector2& p_value);

		/**
		* Convert the given ImVec2 to FVector2
		* @param p_value
		*/
		static OvMaths::FVector2 ToFVector2(const ImVec2& p_value);
	};
}