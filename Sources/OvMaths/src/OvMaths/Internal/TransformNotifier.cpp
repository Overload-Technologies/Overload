/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvMaths/Internal/TransformNotifier.h"

OvMaths::Internal::TransformNotifier::NotificationHandlerID OvMaths::Internal::TransformNotifier::AddNotificationHandler(NotificationHandler p_notificationHandler)
{
	NotificationHandlerID handlerID = m_availableHandlerID++;
	m_notificationHandlers.emplace(handlerID, p_notificationHandler);
	return handlerID;
}

void OvMaths::Internal::TransformNotifier::NotifyChildren(ENotification p_notification)
{
	if (!m_notificationHandlers.empty())
		for (auto const&[id, handler] : m_notificationHandlers)
			handler(p_notification);
}

bool OvMaths::Internal::TransformNotifier::RemoveNotificationHandler(const NotificationHandlerID& p_notificationHandlerID)
{
	return m_notificationHandlers.erase(p_notificationHandlerID) != 0;
}
