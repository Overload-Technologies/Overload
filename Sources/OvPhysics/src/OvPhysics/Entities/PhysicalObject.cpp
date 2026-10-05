/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <OvDebug/Assertion.h>
#include <OvDebug/Logger.h>
#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Tools/Conversion.h>

using namespace OvPhysics::Tools;
using namespace OvPhysics::Settings;

OvTools::Eventing::Event<OvPhysics::Entities::PhysicalObject&>	OvPhysics::Entities::PhysicalObject::CreatedEvent;
OvTools::Eventing::Event<OvPhysics::Entities::PhysicalObject&>	OvPhysics::Entities::PhysicalObject::DestroyedEvent;

namespace
{
	constexpr float kMinimumMass = 0.0000001f;

	JPH::EMotionQuality ToMotionQuality(OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode p_mode)
	{
		switch (p_mode)
		{
		case OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode::CONTINUOUS:
			return JPH::EMotionQuality::LinearCast;
		default:
			return JPH::EMotionQuality::Discrete;
		}
	}

	JPH::EAllowedDOFs ToAllowedDOFs(const OvMaths::FVector3& p_linearFactor, const OvMaths::FVector3& p_angularFactor)
	{
		JPH::EAllowedDOFs result = JPH::EAllowedDOFs::None;

		if (p_linearFactor.x != 0.0f)
		{
			result |= JPH::EAllowedDOFs::TranslationX;
		}

		if (p_linearFactor.y != 0.0f)
		{
			result |= JPH::EAllowedDOFs::TranslationY;
		}

		if (p_linearFactor.z != 0.0f)
		{
			result |= JPH::EAllowedDOFs::TranslationZ;
		}

		if (p_angularFactor.x != 0.0f)
		{
			result |= JPH::EAllowedDOFs::RotationX;
		}

		if (p_angularFactor.y != 0.0f)
		{
			result |= JPH::EAllowedDOFs::RotationY;
		}

		if (p_angularFactor.z != 0.0f)
		{
			result |= JPH::EAllowedDOFs::RotationZ;
		}

		return result;
	}
}

OvPhysics::Entities::PhysicalObject::PhysicalObject() :
	m_transform(new OvMaths::FTransform()),
	m_internalTransform(true)
{
	CollisionStartEvent += [this](OvPhysics::Entities::PhysicalObject& otherPhysicalObject)
	{
		UpdateBodyTransform();
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
	if (m_body->IsDynamic())
	{
		m_body->AddForce(Conversion::ToJoltVector3(p_force));
	}
}

void OvPhysics::Entities::PhysicalObject::AddImpulse(const OvMaths::FVector3& p_impulse)
{
	if (m_body->IsDynamic())
	{
		m_body->AddImpulse(Conversion::ToJoltVector3(p_impulse));
	}
}

void OvPhysics::Entities::PhysicalObject::ClearForces()
{
	if (m_body->IsDynamic())
	{
		m_body->ResetForce();
		m_body->ResetTorque();
	}
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
	return m_body->GetRestitution();
}

float OvPhysics::Entities::PhysicalObject::GetFriction() const
{
	return m_body->GetFriction();
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetLinearVelocity() const
{
	return Conversion::ToOvVector3(m_body->GetLinearVelocity());
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetAngularVelocity() const
{
	return Conversion::ToOvVector3(m_body->GetAngularVelocity());
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetLinearFactor() const
{
	return m_linearFactor;
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalObject::GetAngularFactor() const
{
	return m_angularFactor;
}

bool OvPhysics::Entities::PhysicalObject::IsTrigger() const
{
	return m_trigger;
}

bool OvPhysics::Entities::PhysicalObject::IsKinematic() const
{
	return m_kinematic;
}

OvMaths::FTransform& OvPhysics::Entities::PhysicalObject::GetTransform()
{
	return *m_transform;
}

void OvPhysics::Entities::PhysicalObject::SetMass(float p_mass)
{
	m_mass = p_mass;
	RecreateBody();
}

void OvPhysics::Entities::PhysicalObject::SetCollisionDetectionMode(ECollisionDetectionMode p_mode)
{
	m_collisionMode = p_mode;
	m_bodyInterface->SetMotionQuality(m_body->GetID(), ToMotionQuality(m_collisionMode));
}

void OvPhysics::Entities::PhysicalObject::SetBounciness(float p_bounciness)
{
	m_body->SetRestitution(p_bounciness);
}

void OvPhysics::Entities::PhysicalObject::SetFriction(float p_friction)
{
	m_body->SetFriction(p_friction);
}

void OvPhysics::Entities::PhysicalObject::SetLinearVelocity(const OvMaths::FVector3 & p_linearVelocity)
{
	if (m_body->IsDynamic())
	{
		m_body->SetLinearVelocityClamped(Conversion::ToJoltVector3(p_linearVelocity));
	}
}

void OvPhysics::Entities::PhysicalObject::SetAngularVelocity(const OvMaths::FVector3 & p_angularVelocity)
{
	if (m_body->IsDynamic())
	{
		m_body->SetAngularVelocityClamped(Conversion::ToJoltVector3(p_angularVelocity));
	}
}

void OvPhysics::Entities::PhysicalObject::SetLinearFactor(const OvMaths::FVector3 & p_linearFactor)
{
	m_linearFactor = p_linearFactor;
	RecreateBody();
}

void OvPhysics::Entities::PhysicalObject::SetAngularFactor(const OvMaths::FVector3 & p_angularFactor)
{
	m_angularFactor = p_angularFactor;
	RecreateBody();
}

void OvPhysics::Entities::PhysicalObject::SetTrigger(bool p_trigger)
{
	m_body->SetIsSensor(p_trigger);
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

void OvPhysics::Entities::PhysicalObject::UpdateBodyTransform()
{
	m_bodyInterface->SetPositionAndRotation(
		m_body->GetID(),
		Conversion::ToJoltVector3(m_transform->GetWorldPosition()),
		Conversion::ToJoltQuaternion(m_transform->GetWorldRotation()),
		JPH::EActivation::DontActivate
	);

	if (OvMaths::FVector3::Distance(m_transform->GetWorldScale(), m_previousScale) >= 0.01f)
	{
		RecreateBody();
	}
}

void OvPhysics::Entities::PhysicalObject::UpdateFTransform()
{
	if (m_kinematic)
	{
		return;
	}

	const OvMaths::FVector3 position = Conversion::ToOvVector3(m_body->GetPosition());
	const OvMaths::FQuaternion rotation = Conversion::ToOvQuaternion(m_body->GetRotation());

	// World setters recompute the local transform (and its scale) from the matrices, so they are only used when required
	if (m_transform->HasParent())
	{
		m_transform->SetWorldPosition(position);
		m_transform->SetWorldRotation(rotation);
	}
	else
	{
		m_transform->SetLocalPosition(position);
		m_transform->SetLocalRotation(rotation);
	}
}

void OvPhysics::Entities::PhysicalObject::RecreateBody()
{
	CreateBody(DestroyBody());
}

void OvPhysics::Entities::PhysicalObject::Consider()
{
	if (!m_considered)
	{
		m_considered = true;
		m_bodyInterface->AddBody(m_body->GetID(), JPH::EActivation::Activate);
	}
}

void OvPhysics::Entities::PhysicalObject::Unconsider()
{
	if (m_considered)
	{
		m_considered = false;
		m_bodyInterface->RemoveBody(m_body->GetID());
	}
}

void OvPhysics::Entities::PhysicalObject::CreateBody(const Settings::BodySettings & p_bodySettings)
{
	OVASSERT(m_bodyInterface != nullptr, "A PhysicsEngine must exist before creating a PhysicalObject");

	m_previousScale = m_transform->GetWorldScale();
	m_linearFactor = p_bodySettings.linearFactor;
	m_angularFactor = p_bodySettings.angularFactor;

	// Jolt cannot simulate a dynamic body without any degree of freedom, such a body is simulated as kinematic
	const JPH::EAllowedDOFs allowedDOFs = ToAllowedDOFs(m_linearFactor, m_angularFactor);
	const bool isDynamic = !m_kinematic && allowedDOFs != JPH::EAllowedDOFs::None;

	JPH::BodyCreationSettings bodyCreationSettings(
		CreateShape({ std::abs(m_previousScale.x), std::abs(m_previousScale.y), std::abs(m_previousScale.z) }),
		Conversion::ToJoltVector3(m_transform->GetWorldPosition()),
		Conversion::ToJoltQuaternion(m_transform->GetWorldRotation()),
		isDynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Kinematic,
		kObjectLayer
	);

	bodyCreationSettings.mAllowedDOFs = isDynamic ? allowedDOFs : JPH::EAllowedDOFs::All;
	bodyCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	bodyCreationSettings.mMassPropertiesOverride.mMass = std::max(kMinimumMass, m_mass);
	bodyCreationSettings.mIsSensor = p_bodySettings.isTrigger;
	bodyCreationSettings.mMotionQuality = ToMotionQuality(m_collisionMode);
	bodyCreationSettings.mAllowSleeping = false; // TODO: Avoid using always active
	bodyCreationSettings.mFriction = p_bodySettings.friction;
	bodyCreationSettings.mRestitution = p_bodySettings.restitution;
	bodyCreationSettings.mLinearDamping = 0.0f;
	bodyCreationSettings.mAngularDamping = 0.0f;
	bodyCreationSettings.mUserData = reinterpret_cast<JPH::uint64>(this);

	if (isDynamic)
	{
		bodyCreationSettings.mLinearVelocity = Conversion::ToJoltVector3(p_bodySettings.linearVelocity);
		bodyCreationSettings.mAngularVelocity = Conversion::ToJoltVector3(p_bodySettings.angularVelocity);
	}

	m_body = m_bodyInterface->CreateBody(bodyCreationSettings);

	OVASSERT(m_body != nullptr, "Unable to create a physical body, the maximum number of bodies has been reached");

	if (m_enabled)
		Consider();
}

OvPhysics::Settings::BodySettings OvPhysics::Entities::PhysicalObject::DestroyBody()
{
	BodySettings result
	{
		Conversion::ToOvVector3(m_body->GetLinearVelocity()),
		Conversion::ToOvVector3(m_body->GetAngularVelocity()),
		m_linearFactor,
		m_angularFactor,
		GetBounciness(),
		GetFriction(),
		IsTrigger(),
		IsKinematic()
	};

	Unconsider();

	m_bodyInterface->DestroyBody(m_body->GetID());
	m_body = nullptr;

	return result;
}
