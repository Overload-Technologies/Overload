/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvPhysics/Entities/PhysicalObject.h"

namespace OvPhysics::Entities
{
	/**
	* PhysicalObject with a capsule shape
	*/
	class PhysicalCapsule : public PhysicalObject
	{
	public:
		/**
		* PhysicalCapsule constructor (Internal transform management)
		* @param p_radius
		* @param p_height
		*/
		PhysicalCapsule(float p_radius = 1.f, float p_height = 1.f);

		/**
		* PhysicalCapsule constructor (External transform management)
		* @param p_transform
		* @param p_radius
		* @param p_height
		*/
		PhysicalCapsule(OvMaths::FTransform& p_transform, float p_radius = 1.f, float p_height = 1.f);

		/**
		* Defines a new radius for the capsule
		* @param p_radius
		*/
		void SetRadius(float p_radius);

		/**
		* Defines a new height for the capsule
		* @param p_radius
		*/
		void SetHeight(float p_height);

		/**
		* Returns the actual radius of the capsule
		*/
		float GetRadius() const;

		/**
		* Returns the actual height of the capsule
		*/
		float GetHeight() const;

	private:
		void CreateCollisionShape(float p_radius, float p_height);
		void RecreateCollisionShape(float p_radius, float p_height);
		virtual void SetLocalScaling(const OvMaths::FVector3& p_scaling) override;

	private:
		float m_radius;
		float m_height;
	};
}