/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <OvDebug/Logger.h>
#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Tools/Conversion.h>

using namespace OvPhysics::Tools;
using namespace OvPhysics::Settings;

OvTools::Eventing::Event<OvPhysics::Entities::PhysicalObject&>	OvPhysics::Entities::PhysicalObject::CreatedEvent;
OvTools::Eventing::Event<OvPhysics::Entities::PhysicalObject&>	OvPhysics::Entities::PhysicalObject::DestroyedEvent;
OvTools::Eventing::Event<btRigidBody&>							OvPhysics::Entities::PhysicalObject::ConsiderEvent;
OvTools::Eventing::Event<btRigidBody&>							OvPhysics::Entities::PhysicalObject::UnconsiderEvent;

namespace
{
	void AddFlag(btCollisionObject& p_object, btCollisionObject::CollisionFlags p_flag)
	{
		p_object.setCollisionFlags(p_object.getCollisionFlags() | p_flag);
	}

	void RemoveFlag(btCollisionObject& p_object, btCollisionObject::CollisionFlags p_flag)
	{
		p_object.setCollisionFlags(p_object.getCollisionFlags() & ~p_flag);
	}
}

OvPhysics::Entities::PhysicalObject::PhysicalObject() :
	m_transform(new OvMaths::FTransform()),
	m_internalTransform(true)
{
	CollisionStartEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		UpdateBtTransform();
	};
}

OvPhysics::Entities::PhysicalObject::PhysicalObject(OvMaths::FTransform& p_transform) :
	m_transform(&p_transform),
	m_internalTransform(false)
{

}

OvPhysics::Entities::PhysicalObject::~PhysicalObject()
{
	DestroyBody();
	PhysicalObject::DestroyedEvent.Invoke(*this);

	if (m_internalTransform)
		delete m_transform;
}

void OvPhysics::Entities::PhysicalObject::Init()
{
	PhysicalObject::CreatedEvent.Invoke(*this);
	CreateBody({});
}

void OvPhysics::Entities::PhysicalObject::AddForce(const OvMaths::FVector3& p_force)
{
	m_body->applyCentralForce(Conversion::ToBtVector3(p_force));
}

void OvPhysics::Entities::PhysicalObject::AddImpulse(const OvMaths::FVector3& p_impulse)
{
	m_body->applyCentralImpulse(Conversion::ToBtVector3(p_impulse));
}

void OvPhysics::Entities::PhysicalObject::ClearForces()
{
	m_body->clearForces();
}

float OvPhysics::Entities::PhysicalObject::GetMass() const
{
	return m_mass;
}

const OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode& OvPhysics::Entities::PhysicalObject::GetCollisionDetectionMode() const
{
	return m_collisionMode;
}

float OvPhysics::Entities::PhysicalObject::GetBounciness() const
{
	return m_body->getRestitution();
}

float OvPhysics::Entities::PhysicalObject::GetFriction() const
{
	return m_body->getFriction();
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetLinearVelocity() const
{
	return Conversion::ToOvVector3(m_body->getLinearVelocity());
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetAngularVelocity() const
{
	return Conversion::ToOvVector3(m_body->getAngularVelocity());
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetLinearFactor() const
{
	return Conversion::ToOvVector3(m_body->getLinearFactor());
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetAngularFactor() const
{
	return Conversion::ToOvVector3(m_body->getAngularFactor());
}

bool OvPhysics::Entities::PhysicalObject::IsTrigger() const
{
	return m_trigger;
}

bool OvPhysics::Entities::PhysicalObject::IsKinematic() const
{
	return m_kinematic;
}

OvPhysics::Entities::PhysicalObject::EActivationState OvPhysics::Entities::PhysicalObject::GetActivationState() const
{
	return static_cast<EActivationState>(m_body->getActivationState());
}

OvMaths::FTransform& OvPhysics::Entities::PhysicalObject::GetTransform()
{
	return *m_transform;
}

void OvPhysics::Entities::PhysicalObject::SetMass(float p_mass)
{
	m_mass = p_mass;
	ApplyInertia();
}

void OvPhysics::Entities::PhysicalObject::SetCollisionDetectionMode(ECollisionDetectionMode p_mode)
{
	m_collisionMode = p_mode;

	switch (m_collisionMode)
	{
	case ECollisionDetectionMode::DISCRETE:
		m_body->setCcdMotionThreshold(std::numeric_limits<float>::max());
		m_body->setCcdSweptSphereRadius(0.0f);
		break;
	case ECollisionDetectionMode::CONTINUOUS:
		m_body->setCcdMotionThreshold(static_cast<btScalar>(1e-7));
		m_body->setCcdSweptSphereRadius(0.5f);
		break;
	}
}

void OvPhysics::Entities::PhysicalObject::SetBounciness(float p_bounciness)
{
	m_body->setRestitution(p_bounciness);
}

void OvPhysics::Entities::PhysicalObject::SetFriction(float p_friction)
{
	m_body->setFriction(p_friction);
}

void OvPhysics::Entities::PhysicalObject::SetLinearVelocity(const OvMaths::FVector3 & p_linearVelocity)
{
	m_body->setLinearVelocity(Conversion::ToBtVector3(p_linearVelocity));
}

void OvPhysics::Entities::PhysicalObject::SetAngularVelocity(const OvMaths::FVector3 & p_angularVelocity)
{
	m_body->setAngularVelocity(Conversion::ToBtVector3(p_angularVelocity));
}

void OvPhysics::Entities::PhysicalObject::SetLinearFactor(const OvMaths::FVector3 & p_linearFactor)
{
	m_body->setLinearFactor(Conversion::ToBtVector3(p_linearFactor));
}

void OvPhysics::Entities::PhysicalObject::SetAngularFactor(const OvMaths::FVector3 & p_angularFactor)
{
	m_body->setAngularFactor(Conversion::ToBtVector3(p_angularFactor));
}

void OvPhysics::Entities::PhysicalObject::SetTrigger(bool p_trigger)
{
	if (p_trigger)
		AddFlag(*m_body, btCollisionObject::CF_NO_CONTACT_RESPONSE);
	else
		RemoveFlag(*m_body, btCollisionObject::CF_NO_CONTACT_RESPONSE);

	m_trigger = p_trigger;
}

void OvPhysics::Entities::PhysicalObject::SetKinematic(bool p_kinematic)
{
	m_kinematic = p_kinematic;

	if (m_kinematic)
	{
		ClearForces();
		SetLinearVelocity(OvMaths::FVector3::Zero);
		SetAngularVelocity(OvMaths::FVector3::Zero);
	}

	RecreateBody();
}

void OvPhysics::Entities::PhysicalObject::SetActivationState(EActivationState p_activationState)
{
	m_body->setActivationState(static_cast<int>(p_activationState));
}

void OvPhysics::Entities::PhysicalObject::SetEnabled(bool p_enabled)
{
	m_enabled = p_enabled;

	if (!m_enabled)
		Unconsider();
	else
		Consider();
}

bool OvPhysics::Entities::PhysicalObject::IsEnabled() const
{
	return m_enabled;
}

void OvPhysics::Entities::PhysicalObject::UpdateBtTransform()
{
	m_body->setWorldTransform(Conversion::ToBtTransform(*m_transform));

	if (OvMaths::FVector3::Distance(m_transform->GetWorldScale(), m_previousScale) >= 0.01f)
	{
		m_previousScale = m_transform->GetWorldScale();
		SetLocalScaling({ abs(m_previousScale.x), abs(m_previousScale.y), abs(m_previousScale.z) });
		RecreateBody();
	}
}

void OvPhysics::Entities::PhysicalObject::UpdateFTransform()
{
	if (!m_kinematic)
	{
		const btTransform& result = m_body->getWorldTransform();
		m_transform->SetLocalPosition(Conversion::ToOvVector3(result.getOrigin()));
		m_transform->SetLocalRotation(Conversion::ToOvQuaternion(result.getRotation()));
	}
}

void OvPhysics::Entities::PhysicalObject::RecreateBody()
{
	CreateBody(DestroyBody());
}

void OvPhysics::Entities::PhysicalObject::ApplyInertia()
{
	m_body->setMassProps(
		m_kinematic ? 0.0f : std::max(0.0000001f, m_mass),
		m_kinematic ? btVector3(0.0f, 0.0f, 0.0f) : Tools::Conversion::ToBtVector3(CalculateInertia()));
}

void OvPhysics::Entities::PhysicalObject::Consider()
{
	if (!m_considered)
	{
		m_considered = true;
		ConsiderEvent.Invoke(*m_body);
	}
}

void OvPhysics::Entities::PhysicalObject::Unconsider()
{
	if (m_considered)
	{
		m_considered = false;
		UnconsiderEvent.Invoke(*m_body);
	}
}

void OvPhysics::Entities::PhysicalObject::CreateBody(const Settings::BodySettings & p_bodySettings)
{
	m_motion = std::make_unique<btDefaultMotionState>(Conversion::ToBtTransform(*m_transform));

	m_body = std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo{ 0.0f, m_motion.get(), m_shape.get(), btVector3(0.0f, 0.0f, 0.0f) });

	ApplyInertia();

	m_body->setRestitution(p_bodySettings.restitution);
	m_body->setFriction(p_bodySettings.friction);
	m_body->setLinearVelocity(Conversion::ToBtVector3(p_bodySettings.linearVelocity));
	m_body->setAngularVelocity(Conversion::ToBtVector3(p_bodySettings.angularVelocity));
	m_body->setLinearFactor(Conversion::ToBtVector3(p_bodySettings.linearFactor));
	m_body->setAngularFactor(Conversion::ToBtVector3(p_bodySettings.angularFactor));
	m_body->setUserPointer(this);

	AddFlag(*m_body, btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	if (p_bodySettings.isTrigger)
		AddFlag(*m_body, btCollisionObject::CF_NO_CONTACT_RESPONSE);

	SetActivationState(EActivationState::ALWAYS_ACTIVE); // TODO: Avoid using always active

	if (m_enabled)
		Consider();
}

OvPhysics::Settings::BodySettings OvPhysics::Entities::PhysicalObject::DestroyBody()
{
	BodySettings result
	{
		Conversion::ToOvVector3(m_body->getLinearVelocity()),
		Conversion::ToOvVector3(m_body->getAngularVelocity()),
		Conversion::ToOvVector3(m_body->getLinearFactor()),
		Conversion::ToOvVector3(m_body->getAngularFactor()),
		GetBounciness(),
		GetFriction(),
		IsTrigger(),
		IsKinematic()
	};

	Unconsider();

	m_body.reset();
	m_motion.reset();

	return result;
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::CalculateInertia() const
{
	btVector3 result = { 0.f, 0.f, 0.f };

	if (m_mass != 0.0f)
		m_shape->calculateLocalInertia(m_mass, result);

	return Conversion::ToOvVector3(result);
}

btRigidBody& OvPhysics::Entities::PhysicalObject::GetBody()
{
	return *m_body;
}
