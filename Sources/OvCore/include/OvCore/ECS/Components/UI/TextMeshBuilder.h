/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>
#include <string_view>

#include <OvMaths/FVector2.h>
#include <OvRendering/Resources/Font.h>
#include <OvRendering/Resources/Mesh.h>

namespace OvCore::ECS::Components::UI
{
	/**
	* Builds text layout geometry from resolved font data.
	*/
	class TextMeshBuilder
	{
	public:
		enum class EHorizontalAlignment
		{
			LEFT,
			CENTER,
			RIGHT
		};

		enum class EVerticalAlignment
		{
			TOP,
			CENTER,
			BOTTOM
		};

		struct Input
		{
			std::string_view text;
			OvRendering::Resources::Font* font = nullptr;
			float fontSize = 1.0f;
			OvMaths::FVector2 uiSize = OvMaths::FVector2::Zero;
			EHorizontalAlignment horizontalAlignment = EHorizontalAlignment::LEFT;
			EVerticalAlignment verticalAlignment = EVerticalAlignment::TOP;
			bool buildMesh = true;
		};

		struct Output
		{
			OvMaths::FVector2 size = OvMaths::FVector2::Zero;
			std::unique_ptr<OvRendering::Resources::Mesh> mesh;
		};

		static Output Build(const Input& p_input);
	};
}
