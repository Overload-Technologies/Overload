/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvCore/ECS/Actor.h"

namespace OvCore::ECS
{
	template<typename T, typename ...Args>
	inline T& OvCore::ECS::Actor::AddComponent(Args&& ...p_args)
	{
		static_assert(std::is_base_of<Components::AComponent, T>::value, "T should derive from AComponent");

		if (auto found = GetComponent<T>(); !found)
		{
			m_components.insert(m_components.begin(), std::make_shared<T>(*this, p_args...));
			T& instance = *dynamic_cast<T*>(m_components.front().get());
			ComponentAddedEvent.Invoke(instance);
			if (m_playing && IsActive())
			{
				reinterpret_cast<OvCore::ECS::Components::AComponent&>(instance).OnAwake();
				reinterpret_cast<OvCore::ECS::Components::AComponent&>(instance).OnEnable();
				reinterpret_cast<OvCore::ECS::Components::AComponent&>(instance).OnStart();
			}
			return instance;
		}
		else
		{
			return *found;
		}
	}

	template<typename T>
	inline bool OvCore::ECS::Actor::RemoveComponent()
	{
		static_assert(std::is_base_of<Components::AComponent, T>::value, "T should derive from AComponent");
		static_assert(!std::is_same<Components::CTransform, T>::value, "You can't remove a CTransform from an actor");

		std::shared_ptr<T> result(nullptr);

		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			result = std::dynamic_pointer_cast<T>(*it);
			if (result)
			{
				ComponentRemovedEvent.Invoke(*result.get());
				m_components.erase(it);
				return true;
			}
		}

		return false;
	}

	template<typename T>
	inline T* Actor::GetComponent() const
	{
		static_assert(std::is_base_of<Components::AComponent, T>::value, "T should derive from AComponent");

		std::shared_ptr<T> result(nullptr);

		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			result = std::dynamic_pointer_cast<T>(*it);
			if (result)
			{
				return result.get();
			}
		}

		return nullptr;
	}
}