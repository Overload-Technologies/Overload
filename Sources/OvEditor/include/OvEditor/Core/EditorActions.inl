/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvEditor/Core/EditorActions.h>

namespace OvEditor::Core
{
	template<typename T>
	inline OvCore::ECS::Actor & EditorActions::CreateMonoComponentActor(bool p_focusOnCreation, OvCore::ECS::Actor* p_parent)
	{
		auto& instance = CreateEmptyActor(false, p_parent);

		T& component = instance.AddComponent<T>();

		instance.SetName(component.GetName());

		if (p_focusOnCreation)
			SelectActor(instance);

		return instance;
	}
}
