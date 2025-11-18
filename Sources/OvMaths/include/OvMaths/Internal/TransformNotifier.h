/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <functional>

namespace OvMaths::Internal
{
	/**
	* The TransformNotifier is a simple notification system used by transform to send notifications to his children
	*/
	class TransformNotifier
	{
	public:
		/**
		* Notifications that the transform can send
		*/
		enum class ENotification
		{
			TRANSFORM_CHANGED,
			TRANSFORM_DESTROYED
		};

		/**
		* The NotificationHandler is a function callback that takes a ENotification in parameters
		*/
		using NotificationHandler = std::function<void(ENotification)>;

		/**
		* The ID of a NotificationHandler
		* This value is needed to remove a NotificationHandler from a Notifier
		*/
		using NotificationHandlerID = uint64_t;

		/**
		* Add a NotificationHandler to the Notifier. The NotificationHandler will receive
		* every notifications sent by this Notifier.
		* This method also return a NotificationHandlerID needed to remove the handler later
		* @param p_notificationHandler
		*/
		NotificationHandlerID AddNotificationHandler(NotificationHandler p_notificationHandler);

		/**
		* Notify children (NotificationHandler) by sending the given notification
		* @param p_notification
		*/
		void NotifyChildren(ENotification p_notification);

		/**
		* Remove a NotificationHandler to the Notifier using the NotificationHandlerID generated
		* by the "AddNotificationHandler" method.
		* This method returns true on success (NotificationHandler removed)
		* @param p_notificationHandlerID
		*/
		bool RemoveNotificationHandler(const NotificationHandlerID& p_notificationHandlerID);

	private:
		std::unordered_map<NotificationHandlerID, NotificationHandler> m_notificationHandlers;
		NotificationHandlerID m_availableHandlerID = 0;
	};
}