/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <OvCore/ECS/Components/UI/TextLayoutEngine.h>
#include <OvRendering/Resources/Mesh.h>

namespace OvCore::ECS::Components::UI
{
	/**
	* Builds text layout geometry from resolved font data.
	*/
	class TextMeshBuilder
	{
	public:
		using EHorizontalAlignment = TextLayoutEngine::EHorizontalAlignment;
		using EVerticalAlignment = TextLayoutEngine::EVerticalAlignment;
		using Input = TextLayoutEngine::Input;

		struct Output
		{
			OvMaths::FVector2 size = OvMaths::FVector2::Zero;
			std::unique_ptr<OvRendering::Resources::Mesh> mesh;
		};

		static Output Build(const Input& p_input);
		static Output Build(const TextLayoutEngine::Output& p_layout);
	};
}
