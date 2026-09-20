/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <algorithm>

#include "OvTools/Eventing/Event.h"

namespace OvTools::Eventing
{
	template<class... ArgTypes>
	ListenerID Event<ArgTypes...>::AddListener(Callback p_callback)
	{
		ListenerID listenerID = m_availableListenerID++;

		if (m_invokeDepth > 0)
		{
			m_pendingAdditions.emplace_back(listenerID, std::move(p_callback));
		}
		else
		{
			m_callbacks.emplace(listenerID, std::move(p_callback));
		}

		return listenerID;
	}

	template<class... ArgTypes>
	ListenerID Event<ArgTypes...>::operator+=(Callback p_callback)
	{
		return AddListener(std::move(p_callback));
	}

	template<class... ArgTypes>
	bool Event<ArgTypes...>::RemoveListener(ListenerID p_listenerID)
	{
		if (m_invokeDepth == 0)
		{
			return m_callbacks.erase(p_listenerID) != 0;
		}

		// Deferring the erase keeps a self-removing callback alive while it runs, and keeps the
		// callback map stable while it is being iterated
		if (m_callbacks.contains(p_listenerID) && !IsPendingRemoval(p_listenerID))
		{
			m_pendingRemovals.push_back(p_listenerID);
			return true;
		}

		const auto pendingAddition = std::find_if(
			m_pendingAdditions.begin(),
			m_pendingAdditions.end(),
			[p_listenerID](const auto& p_addition) { return p_addition.first == p_listenerID; }
		);

		if (pendingAddition != m_pendingAdditions.end())
		{
			m_pendingAdditions.erase(pendingAddition);
			return true;
		}

		return false;
	}

	template<class... ArgTypes>
	bool Event<ArgTypes...>::operator-=(ListenerID p_listenerID)
	{
		return RemoveListener(p_listenerID);
	}

	template<class... ArgTypes>
	void Event<ArgTypes...>::RemoveAllListeners()
	{
		if (m_invokeDepth == 0)
		{
			m_callbacks.clear();
			return;
		}

		m_pendingAdditions.clear();

		for (const auto& listener : m_callbacks)
		{
			if (!IsPendingRemoval(listener.first))
			{
				m_pendingRemovals.push_back(listener.first);
			}
		}
	}

	template<class... ArgTypes>
	uint64_t Event<ArgTypes...>::GetListenerCount()
	{
		return m_callbacks.size() + m_pendingAdditions.size() - m_pendingRemovals.size();
	}

	template<class... ArgTypes>
	void Event<ArgTypes...>::Invoke(ArgTypes... p_args)
	{
		// Additions and removals are deferred for the whole dispatch, so the callback map stays
		// valid for the entire iteration, even on a nested invoke
		++m_invokeDepth;

		for (const auto& [listenerID, callback] : m_callbacks)
		{
			if (callback && !IsPendingRemoval(listenerID))
			{
				callback(p_args...);
			}
		}

		--m_invokeDepth;

		if (m_invokeDepth == 0)
		{
			FlushPendingOperations();
		}
	}

	template<class... ArgTypes>
	bool Event<ArgTypes...>::IsPendingRemoval(ListenerID p_listenerID) const
	{
		return std::find(m_pendingRemovals.begin(), m_pendingRemovals.end(), p_listenerID) != m_pendingRemovals.end();
	}

	template<class... ArgTypes>
	void Event<ArgTypes...>::FlushPendingOperations()
	{
		for (const auto listenerID : m_pendingRemovals)
		{
			m_callbacks.erase(listenerID);
		}

		m_pendingRemovals.clear();

		for (auto& pendingAddition : m_pendingAdditions)
		{
			m_callbacks.emplace(pendingAddition.first, std::move(pendingAddition.second));
		}

		m_pendingAdditions.clear();
	}
}
