/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CTransform.h>

OvCore::ECS::Components::CTransform::CTransform(ECS::Actor& p_owner, OvMaths::FVector3 p_localPosition, OvMaths::FQuaternion p_localRotation, OvMaths::FVector3 p_localScale) :
AComponent(p_owner)
{
	m_transform.GenerateMatricesLocal(p_localPosition, p_localRotation, p_localScale);
}

std::string OvCore::ECS::Components::CTransform::GetName()
{
	return "Transform";
}

std::string OvCore::ECS::Components::CTransform::GetTypeName()
{
	return std::string{ComponentTraits<CTransform>::Name};
}

void OvCore::ECS::Components::CTransform::SetParent(CTransform& p_parent)
{
	m_transform.SetParent(p_parent.GetFTransform());
}

bool OvCore::ECS::Components::CTransform::RemoveParent()
{
	return m_transform.RemoveParent();
}

bool OvCore::ECS::Components::CTransform::HasParent() const
{
	return m_transform.HasParent();
}

void OvCore::ECS::Components::CTransform::SetLocalPosition(OvMaths::FVector3 p_newPosition)
{
	m_transform.SetLocalPosition(p_newPosition);
}

void OvCore::ECS::Components::CTransform::SetLocalRotation(OvMaths::FQuaternion p_newRotation)
{
	m_transform.SetLocalRotation(p_newRotation);
}

void OvCore::ECS::Components::CTransform::SetLocalScale(OvMaths::FVector3 p_newScale)
{
	m_transform.SetLocalScale(p_newScale);
}

void OvCore::ECS::Components::CTransform::SetWorldPosition(OvMaths::FVector3 p_newPosition)
{
	m_transform.SetWorldPosition(p_newPosition);
}

void OvCore::ECS::Components::CTransform::SetWorldRotation(OvMaths::FQuaternion p_newRotation)
{
	m_transform.SetWorldRotation(p_newRotation);
}

void OvCore::ECS::Components::CTransform::SetWorldScale(OvMaths::FVector3 p_newScale)
{
	m_transform.SetWorldScale(p_newScale);
}

void OvCore::ECS::Components::CTransform::TranslateLocal(const OvMaths::FVector3 & p_translation)
{
	m_transform.TranslateLocal(p_translation);
}

void OvCore::ECS::Components::CTransform::RotateLocal(const OvMaths::FQuaternion & p_rotation)
{
	m_transform.RotateLocal(p_rotation);
}

void OvCore::ECS::Components::CTransform::ScaleLocal(const OvMaths::FVector3 & p_scale)
{
	m_transform.ScaleLocal(p_scale);
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetLocalPosition() const
{
	return m_transform.GetLocalPosition();
}

const OvMaths::FQuaternion & OvCore::ECS::Components::CTransform::GetLocalRotation() const
{
	return m_transform.GetLocalRotation();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetLocalScale() const
{
	return m_transform.GetLocalScale();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetWorldPosition() const
{
	return m_transform.GetWorldPosition();
}

const OvMaths::FQuaternion & OvCore::ECS::Components::CTransform::GetWorldRotation() const
{
	return m_transform.GetWorldRotation();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetWorldScale() const
{
	return m_transform.GetWorldScale();
}

const OvMaths::FMatrix4 & OvCore::ECS::Components::CTransform::GetLocalMatrix() const
{
	return m_transform.GetLocalMatrix();
}

const OvMaths::FMatrix4 & OvCore::ECS::Components::CTransform::GetWorldMatrix() const
{
	return m_transform.GetWorldMatrix();
}

OvMaths::FTransform & OvCore::ECS::Components::CTransform::GetFTransform()
{
	return m_transform;
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetWorldForward() const
{
	return m_transform.GetWorldForward();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetWorldUp() const
{
	return m_transform.GetWorldUp();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetWorldRight() const
{
	return m_transform.GetWorldRight();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetLocalForward() const
{
	return m_transform.GetLocalForward();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetLocalUp() const
{
	return m_transform.GetLocalUp();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetLocalRight() const
{
	return m_transform.GetLocalRight();
}

void OvCore::ECS::Components::CTransform::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::SerializeVec3(p_doc, p_node, "position", GetLocalPosition());
	OvCore::Helpers::Serializer::SerializeQuat(p_doc, p_node, "rotation", GetLocalRotation());
	OvCore::Helpers::Serializer::SerializeVec3(p_doc, p_node, "scale", GetLocalScale());
}

void OvCore::ECS::Components::CTransform::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	m_transform.GenerateMatricesLocal
	(
		OvCore::Helpers::Serializer::DeserializeVec3(p_doc, p_node, "position"),
		OvCore::Helpers::Serializer::DeserializeQuat(p_doc, p_node, "rotation"),
		OvCore::Helpers::Serializer::DeserializeVec3(p_doc, p_node, "scale")
	);
}

void OvCore::ECS::Components::CTransform::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	auto getRotation = [this]
	{ 
		return OvMaths::FQuaternion::EulerAngles(GetLocalRotation());
	};

	auto setRotation = [this](OvMaths::FVector3 result)
	{
		SetLocalRotation(OvMaths::FQuaternion(result));
	};

	OvCore::Helpers::GUIDrawer::DrawVec3(p_root, "Position", std::bind(&CTransform::GetLocalPosition, this), std::bind(&CTransform::SetLocalPosition, this, std::placeholders::_1), 0.05f);
	OvCore::Helpers::GUIDrawer::DrawVec3(p_root, "Rotation", getRotation, setRotation, 0.05f);
	OvCore::Helpers::GUIDrawer::DrawVec3(p_root, "Scale", std::bind(&CTransform::GetLocalScale, this), std::bind(&CTransform::SetLocalScale, this, std::placeholders::_1), 0.05f, 0.0001f);
}
