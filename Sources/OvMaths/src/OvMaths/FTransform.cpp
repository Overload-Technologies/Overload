/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvMaths/FTransform.h"

OvMaths::FTransform::FTransform(FVector3 p_localPosition, FQuaternion p_localRotation, FVector3 p_localScale) :
	m_notificationHandlerID(-1),
	m_parent(nullptr)
{
	GenerateMatricesLocal(p_localPosition, p_localRotation, p_localScale);
}

OvMaths::FTransform::~FTransform()
{
	m_notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED);
}

OvMaths::FTransform::FTransform(const FTransform& p_other) :
	FTransform(p_other.m_worldPosition, p_other.m_worldRotation, p_other.m_worldScale)
{
}

OvMaths::FTransform& OvMaths::FTransform::operator=(const FTransform& p_other)
{
	GenerateMatricesWorld(
		p_other.m_worldPosition,
		p_other.m_worldRotation,
		p_other.m_worldScale
	);

	return *this;
}

void OvMaths::FTransform::NotificationHandler(Internal::TransformNotifier::ENotification p_notification)
{
	switch (p_notification)
	{
	case Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED:
		UpdateWorldMatrix();
		break;

	case Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED:
		/* 
		* RemoveParent() is not called here because it is unsafe to remove a notification handler
		* while the parent is iterating on his notification handlers (Segfault otherwise)
		*/
		GenerateMatricesLocal(m_worldPosition, m_worldRotation, m_worldScale);
		m_parent = nullptr;
		UpdateWorldMatrix();
		break;
	}
}

void OvMaths::FTransform::SetParent(FTransform& p_parent)
{
	m_parent = &p_parent;

	m_notificationHandlerID = m_parent->m_notifier.AddNotificationHandler(std::bind(&FTransform::NotificationHandler, this, std::placeholders::_1));

	UpdateWorldMatrix();
}

bool OvMaths::FTransform::RemoveParent()
{
	if (m_parent != nullptr)
	{
		m_parent->m_notifier.RemoveNotificationHandler(m_notificationHandlerID);
		m_parent = nullptr;
		UpdateWorldMatrix();

		return true;
	}

	return false;
}

bool OvMaths::FTransform::HasParent() const
{
	return m_parent != nullptr;
}

void OvMaths::FTransform::GenerateMatricesLocal(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale)
{
	m_localMatrix = FMatrix4::Translation(p_position) * FQuaternion::ToMatrix4(FQuaternion::Normalize(p_rotation)) * FMatrix4::Scaling(p_scale);
	m_localPosition = p_position;
	m_localRotation = p_rotation;
	m_localScale = p_scale;

	UpdateWorldMatrix();
}

void OvMaths::FTransform::GenerateMatricesWorld(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale)
{
	m_worldMatrix = FMatrix4::Translation(p_position) * FQuaternion::ToMatrix4(FQuaternion::Normalize(p_rotation)) * FMatrix4::Scaling(p_scale);
	m_worldPosition = p_position;
	m_worldRotation = p_rotation;
	m_worldScale = p_scale;

	UpdateLocalMatrix();
}

void OvMaths::FTransform::UpdateWorldMatrix()
{
	m_worldMatrix = HasParent() ? m_parent->m_worldMatrix * m_localMatrix : m_localMatrix;
	PreDecomposeWorldMatrix();

	m_notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void OvMaths::FTransform::UpdateLocalMatrix()
{
	m_localMatrix = HasParent() ? FMatrix4::Inverse(m_parent->m_worldMatrix) * m_worldMatrix : m_worldMatrix;
	PreDecomposeLocalMatrix();

	m_notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void OvMaths::FTransform::SetLocalPosition(FVector3 p_newPosition)
{
	GenerateMatricesLocal(p_newPosition, m_localRotation, m_localScale);
}

void OvMaths::FTransform::SetLocalRotation(FQuaternion p_newRotation)
{
	GenerateMatricesLocal(m_localPosition, p_newRotation, m_localScale);
}

void OvMaths::FTransform::SetLocalScale(FVector3 p_newScale)
{
	GenerateMatricesLocal(m_localPosition, m_localRotation, p_newScale);
}

void OvMaths::FTransform::SetWorldPosition(FVector3 p_newPosition)
{
	GenerateMatricesWorld(p_newPosition, m_worldRotation, m_worldScale);
}

void OvMaths::FTransform::SetWorldRotation(FQuaternion p_newRotation)
{
	GenerateMatricesWorld(m_worldPosition, p_newRotation, m_worldScale);
}

void OvMaths::FTransform::SetWorldScale(FVector3 p_newScale)
{
	GenerateMatricesWorld(m_worldPosition, m_worldRotation, p_newScale);
}

void OvMaths::FTransform::TranslateLocal(const FVector3& p_translation)
{
	SetLocalPosition(m_localPosition + p_translation);
}

void OvMaths::FTransform::RotateLocal(const FQuaternion& p_rotation)
{
	SetLocalRotation(m_localRotation * p_rotation);
}

void OvMaths::FTransform::ScaleLocal(const FVector3& p_scale)
{
	SetLocalScale(FVector3
	(
		m_localScale.x * p_scale.x,
		m_localScale.y * p_scale.y,
		m_localScale.z * p_scale.z
	));
}

const OvMaths::FVector3& OvMaths::FTransform::GetLocalPosition() const
{
	return m_localPosition;
}

const OvMaths::FQuaternion& OvMaths::FTransform::GetLocalRotation() const
{
	return m_localRotation;
}

const OvMaths::FVector3& OvMaths::FTransform::GetLocalScale() const
{
	return m_localScale;
}

const OvMaths::FVector3& OvMaths::FTransform::GetWorldPosition() const
{
	return m_worldPosition;
}

const OvMaths::FQuaternion& OvMaths::FTransform::GetWorldRotation() const
{
	return m_worldRotation;
}

const OvMaths::FVector3& OvMaths::FTransform::GetWorldScale() const
{
	return m_worldScale;
}

const OvMaths::FMatrix4& OvMaths::FTransform::GetLocalMatrix() const
{
	return m_localMatrix;
}

const OvMaths::FMatrix4& OvMaths::FTransform::GetWorldMatrix() const
{
	return m_worldMatrix;
}

OvMaths::FVector3 OvMaths::FTransform::GetWorldForward() const
{
	return m_worldRotation * FVector3::Forward;
}

OvMaths::FVector3 OvMaths::FTransform::GetWorldUp() const
{
	return m_worldRotation * FVector3::Up;
}

OvMaths::FVector3 OvMaths::FTransform::GetWorldRight() const
{
	return m_worldRotation * FVector3::Right;
}

OvMaths::FVector3 OvMaths::FTransform::GetLocalForward() const
{
	return m_localRotation * FVector3::Forward;
}

OvMaths::FVector3 OvMaths::FTransform::GetLocalUp() const
{
	return m_localRotation * FVector3::Up;
}

OvMaths::FVector3 OvMaths::FTransform::GetLocalRight() const
{
	return m_localRotation * FVector3::Right;
}

void OvMaths::FTransform::PreDecomposeWorldMatrix()
{
	m_worldPosition.x = m_worldMatrix(0, 3);
	m_worldPosition.y = m_worldMatrix(1, 3);
	m_worldPosition.z = m_worldMatrix(2, 3);

	FVector3 columns[3] = 
	{
		{ m_worldMatrix(0, 0), m_worldMatrix(1, 0), m_worldMatrix(2, 0)},
		{ m_worldMatrix(0, 1), m_worldMatrix(1, 1), m_worldMatrix(2, 1)},
		{ m_worldMatrix(0, 2), m_worldMatrix(1, 2), m_worldMatrix(2, 2)},
	};

	m_worldScale.x = FVector3::Length(columns[0]);
	m_worldScale.y = FVector3::Length(columns[1]);
	m_worldScale.z = FVector3::Length(columns[2]);

	if (m_worldScale.x)
	{
		columns[0] /= m_worldScale.x;
	}
	if (m_worldScale.y)
	{
		columns[1] /= m_worldScale.y;
	}
	if (m_worldScale.z)
	{
		columns[2] /= m_worldScale.z;
	}

	FMatrix3 rotationMatrix
	(
		columns[0].x, columns[1].x, columns[2].x,
		columns[0].y, columns[1].y, columns[2].y,
		columns[0].z, columns[1].z, columns[2].z
	);

	m_worldRotation = FQuaternion(rotationMatrix);
}

void OvMaths::FTransform::PreDecomposeLocalMatrix()
{
	m_localPosition.x = m_localMatrix(0, 3);
	m_localPosition.y = m_localMatrix(1, 3);
	m_localPosition.z = m_localMatrix(2, 3);

	FVector3 columns[3] =
	{
		{ m_localMatrix(0, 0), m_localMatrix(1, 0), m_localMatrix(2, 0)},
		{ m_localMatrix(0, 1), m_localMatrix(1, 1), m_localMatrix(2, 1)},
		{ m_localMatrix(0, 2), m_localMatrix(1, 2), m_localMatrix(2, 2)},
	};

	m_localScale.x = FVector3::Length(columns[0]);
	m_localScale.y = FVector3::Length(columns[1]);
	m_localScale.z = FVector3::Length(columns[2]);

	if (m_localScale.x)
	{
		columns[0] /= m_localScale.x;
	}
	if (m_localScale.y)
	{
		columns[1] /= m_localScale.y;
	}
	if (m_localScale.z)
	{
		columns[2] /= m_localScale.z;
	}

	FMatrix3 rotationMatrix
	(
		columns[0].x, columns[1].x, columns[2].x,
		columns[0].y, columns[1].y, columns[2].y,
		columns[0].z, columns[1].z, columns[2].z
	);

	m_localRotation = FQuaternion(rotationMatrix);
}