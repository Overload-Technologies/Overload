/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <functional>

namespace OvTools::Eventing
{
	/**
	* The ID of a listener (Registered callback).
	* This value is needed to remove a listener from an event
	*/
	using ListenerID = uint64_t;

	/**
	* A simple event that contains a set of function callbacks. These functions will be called on invoke
	*/
	template<class... ArgTypes>
	class Event
	{
	public:
		/**
		* Simple shortcut for a generic function without return value
		*/
		using Callback = std::function<void(ArgTypes...)>;

		/**
		* Add a function callback to this event
		* Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
		* @param p_call
		*/
		ListenerID AddListener(Callback p_callback);

		/**
		* Add a function callback to this event
		* Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
		* @param p_call
		*/
		ListenerID operator+=(Callback p_callback);

		/**
		* Remove a function callback to this event using a Listener (Created when calling AddListener)
		* @param p_listener
		*/
		bool RemoveListener(ListenerID p_listenerID);

		/**
		* Remove a function callback to this event using a Listener (Created when calling AddListener)
		* @param p_listener
		*/
		bool operator-=(ListenerID p_listenerID);

		/**
		* Remove every listeners to this event
		*/
		void RemoveAllListeners();

		/**
		* Return the number of callback registered
		*/
		uint64_t GetListenerCount();

		/**
		* Call every callbacks attached to this event
		* @param p_args (Variadic)
		*/
		void Invoke(ArgTypes... p_args);

	private:
		std::unordered_map<ListenerID, Callback>	m_callbacks;
		ListenerID									m_availableListenerID = 0;
	};
}

#include "OvTools/Eventing/Event.inl"