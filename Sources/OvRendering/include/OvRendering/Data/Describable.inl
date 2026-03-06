/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvDebug/Assertion.h>
#include <OvRendering/Data/Describable.h>

namespace OvRendering::Data
{
	template<typename T>
	inline void Describable::AddDescriptor(T&& p_descriptor)
	{
		OVASSERT(!HasDescriptor<T>(), "Descriptor already added");
		m_descriptors.emplace(typeid(T), std::move(p_descriptor));
	}

	template<typename T>
	inline void Describable::RemoveDescriptor()
	{
		OVASSERT(HasDescriptor<T>(), "Descriptor doesn't exist.");
		if (auto it = m_descriptors.find(typeid(T)); it != m_descriptors.end())
		{
			m_descriptors.erase(it);
		}
	}

	template<typename T>
	inline bool Describable::HasDescriptor() const
	{
		return m_descriptors.contains(typeid(T));
	}

	template<typename T>
	inline const T& Describable::GetDescriptor() const
	{
		auto it = m_descriptors.find(typeid(T));
		OVASSERT(it != m_descriptors.end(), "Couldn't find a descriptor matching the given type T.");
		return std::any_cast<const T&>(it->second);
	}

	template<typename T>
	inline bool Describable::TryGetDescriptor(OvTools::Utils::OptRef<const T>& p_outDescriptor) const
	{
		if (auto it = m_descriptors.find(typeid(T)); it != m_descriptors.end())
		{
			p_outDescriptor = std::any_cast<const T&>(it->second);
			return true;
		}

		return false;
	}
}
