/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>

#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

#include <OvDebug/Logger.h>

#include <OvPhysics/Entities/PhysicalObject.h>

#include "OvCore/ECS/Components/CPhysicalObject.h"
#include "OvCore/ECS/Actor.h"

OvCore::ECS::Components::CPhysicalObject::CPhysicalObject(ECS::Actor & p_owner) : 
	AComponent(p_owner)
{
}

void OvCore::ECS::Components::CPhysicalObject::AddForce(const OvMaths::FVector3 & p_force)
{
	m_physicalObject->AddForce(p_force);
}

void OvCore::ECS::Components::CPhysicalObject::AddImpulse(const OvMaths::FVector3 & p_impulse)
{
	m_physicalObject->AddImpulse(p_impulse);
}

void OvCore::ECS::Components::CPhysicalObject::ClearForces()
{
	m_physicalObject->ClearForces();
}

void OvCore::ECS::Components::CPhysicalObject::SetCollisionDetectionMode(OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode p_mode)
{
	m_physicalObject->SetCollisionDetectionMode(p_mode);
}

OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode OvCore::ECS::Components::CPhysicalObject::GetCollisionDetectionMode() const
{
	return m_physicalObject->GetCollisionDetectionMode();
}

float OvCore::ECS::Components::CPhysicalObject::GetMass() const
{
	return m_physicalObject->GetMass();
}

float OvCore::ECS::Components::CPhysicalObject::GetBounciness() const
{
	return m_physicalObject->GetBounciness();
}

float OvCore::ECS::Components::CPhysicalObject::GetFriction() const
{
	return m_physicalObject->GetFriction();
}

OvMaths::FVector3 OvCore::ECS::Components::CPhysicalObject::GetLinearVelocity() const
{
	return m_physicalObject->GetLinearVelocity();
}

OvMaths::FVector3 OvCore::ECS::Components::CPhysicalObject::GetAngularVelocity() const
{
	return m_physicalObject->GetAngularVelocity();
}

OvMaths::FVector3 OvCore::ECS::Components::CPhysicalObject::GetLinearFactor() const
{
	return m_physicalObject->GetLinearFactor();
}

OvMaths::FVector3 OvCore::ECS::Components::CPhysicalObject::GetAngularFactor() const
{
	return m_physicalObject->GetAngularFactor();
}

bool OvCore::ECS::Components::CPhysicalObject::IsTrigger() const
{
	return m_physicalObject->IsTrigger();
}

bool OvCore::ECS::Components::CPhysicalObject::IsKinematic() const
{
	return m_physicalObject->IsKinematic();
}

OvPhysics::Entities::PhysicalObject::EActivationState OvCore::ECS::Components::CPhysicalObject::GetActivationState() const
{
	return m_physicalObject->GetActivationState();
}

void OvCore::ECS::Components::CPhysicalObject::SetMass(float p_mass)
{
	m_physicalObject->SetMass(p_mass);
}

void OvCore::ECS::Components::CPhysicalObject::SetBounciness(float p_bounciness)
{
	m_physicalObject->SetBounciness(p_bounciness);
}

void OvCore::ECS::Components::CPhysicalObject::SetFriction(float p_friction)
{
	m_physicalObject->SetFriction(p_friction);
}

void OvCore::ECS::Components::CPhysicalObject::SetLinearVelocity(const OvMaths::FVector3 & p_linearVelocity)
{
	m_physicalObject->SetLinearVelocity(p_linearVelocity);
}

void OvCore::ECS::Components::CPhysicalObject::SetAngularVelocity(const OvMaths::FVector3 & p_angularVelocity)
{
	m_physicalObject->SetAngularVelocity(p_angularVelocity);
}

void OvCore::ECS::Components::CPhysicalObject::SetLinearFactor(const OvMaths::FVector3 & p_linearFactor)
{
	m_physicalObject->SetLinearFactor(p_linearFactor);
}

void OvCore::ECS::Components::CPhysicalObject::SetAngularFactor(const OvMaths::FVector3 & p_angularFactor)
{
	m_physicalObject->SetAngularFactor(p_angularFactor);
}

void OvCore::ECS::Components::CPhysicalObject::SetTrigger(bool p_trigger)
{
	m_physicalObject->SetTrigger(p_trigger);
}

void OvCore::ECS::Components::CPhysicalObject::SetKinematic(bool p_kinematic)
{
	m_physicalObject->SetKinematic(p_kinematic);
}

void OvCore::ECS::Components::CPhysicalObject::SetActivationState(OvPhysics::Entities::PhysicalObject::EActivationState p_state)
{
	m_physicalObject->SetActivationState(p_state);
}

void OvCore::ECS::Components::CPhysicalObject::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "is_trigger", IsTrigger());
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "is_kinematic", IsKinematic());
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "bounciness", GetBounciness());
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "mass", GetMass());
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "friction", GetFriction());
	Helpers::Serializer::SerializeVec3(p_doc, p_node, "linear_factor", GetLinearFactor());
	Helpers::Serializer::SerializeVec3(p_doc, p_node, "angular_factor", GetAngularFactor());
	Helpers::Serializer::SerializeInt(p_doc, p_node, "collision_mode", static_cast<int>(GetCollisionDetectionMode()));
}

void OvCore::ECS::Components::CPhysicalObject::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	SetTrigger(Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "is_trigger"));
	SetKinematic(Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "is_kinematic"));
	SetBounciness(Helpers::Serializer::DeserializeFloat(p_doc, p_node, "bounciness"));
	SetMass(Helpers::Serializer::DeserializeFloat(p_doc, p_node, "mass"));
	SetFriction(Helpers::Serializer::DeserializeFloat(p_doc, p_node, "friction"));
	SetLinearFactor(Helpers::Serializer::DeserializeVec3(p_doc, p_node, "linear_factor"));
	SetAngularFactor(Helpers::Serializer::DeserializeVec3(p_doc, p_node, "angular_factor"));
	SetCollisionDetectionMode(static_cast<OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode>(Helpers::Serializer::DeserializeInt(p_doc, p_node, "collision_mode")));
}

void OvCore::ECS::Components::CPhysicalObject::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	Helpers::GUIDrawer::DrawBoolean(p_root, "Trigger", std::bind(&CPhysicalObject::IsTrigger, this), std::bind(&CPhysicalObject::SetTrigger, this, std::placeholders::_1));
	Helpers::GUIDrawer::DrawBoolean(p_root, "Kinematic", std::bind(&CPhysicalObject::IsKinematic, this), std::bind(&CPhysicalObject::SetKinematic, this, std::placeholders::_1));
	Helpers::GUIDrawer::DrawScalar<float>(p_root, "Mass", std::bind(&CPhysicalObject::GetMass, this), std::bind(&CPhysicalObject::SetMass, this, std::placeholders::_1), 0.1f, 0.f, 10000.f);
	Helpers::GUIDrawer::DrawScalar<float>(p_root, "Bounciness", std::bind(&CPhysicalObject::GetBounciness, this), std::bind(&CPhysicalObject::SetBounciness, this, std::placeholders::_1), 0.1f, 0.f, 1.f);
	Helpers::GUIDrawer::DrawScalar<float>(p_root, "Friction", std::bind(&CPhysicalObject::GetFriction, this), std::bind(&CPhysicalObject::SetFriction, this, std::placeholders::_1), 0.1f, 0.f, 1.f);
	Helpers::GUIDrawer::DrawVec3(p_root, "Linear Factor", std::bind(&CPhysicalObject::GetLinearFactor, this), std::bind(&CPhysicalObject::SetLinearFactor, this, std::placeholders::_1), 0.1f, 0.f, 1.f);
	Helpers::GUIDrawer::DrawVec3(p_root, "Angular Factor", std::bind(&CPhysicalObject::GetAngularFactor, this), std::bind(&CPhysicalObject::SetAngularFactor, this, std::placeholders::_1), 0.1f, 0.f, 1.f);
	
	Helpers::GUIDrawer::CreateTitle(p_root, "Collision Mode");
	auto& collisionMode = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetCollisionDetectionMode()));
	collisionMode.choices.emplace(0, "Discrete");
	collisionMode.choices.emplace(1, "Continuous");
	collisionMode.ValueChangedEvent += [this](int p_choice)
	{
		SetCollisionDetectionMode(static_cast<OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode>(p_choice));
	};
}

void OvCore::ECS::Components::CPhysicalObject::Init()
{
	m_physicalObject->SetEnabled(false);
}

void OvCore::ECS::Components::CPhysicalObject::BindListener()
{
	/* Collision Events */
	m_physicalObject->CollisionStartEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		auto& otherObject = otherPhysicalObject.GetUserData<std::reference_wrapper<CPhysicalObject>>().get();

		CollisionEnterEvent.Invoke(otherObject);
		owner.OnCollisionEnter(otherObject);
	};
	m_physicalObject->CollisionStayEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		auto& otherObject = otherPhysicalObject.GetUserData<std::reference_wrapper<CPhysicalObject>>().get();

		CollisionStayEvent.Invoke(otherObject);
		owner.OnCollisionStay(otherObject);
	};
	m_physicalObject->CollisionStopEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		auto& otherObject = otherPhysicalObject.GetUserData<std::reference_wrapper<CPhysicalObject>>().get();

		CollisionExitEvent.Invoke(otherObject);
		owner.OnCollisionExit(otherObject);
	};

	/* Trigger Events */
	m_physicalObject->TriggerStartEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		auto& otherObject = otherPhysicalObject.GetUserData<std::reference_wrapper<CPhysicalObject>>().get();

		TriggerEnterEvent.Invoke(otherObject);
		owner.OnTriggerEnter(otherObject);
	};
	m_physicalObject->TriggerStayEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		auto& otherObject = otherPhysicalObject.GetUserData<std::reference_wrapper<CPhysicalObject>>().get();

		TriggerStayEvent.Invoke(otherObject);
		owner.OnTriggerStay(otherObject);
	};
	m_physicalObject->TriggerStopEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		auto& otherObject = otherPhysicalObject.GetUserData<std::reference_wrapper<CPhysicalObject>>().get();

		TriggerExitEvent.Invoke(otherObject);
		owner.OnTriggerExit(otherObject);
	};
}

void OvCore::ECS::Components::CPhysicalObject::OnEnable()
{
	m_physicalObject->SetEnabled(true);
}

void OvCore::ECS::Components::CPhysicalObject::OnDisable()
{
	m_physicalObject->SetEnabled(false);
}
