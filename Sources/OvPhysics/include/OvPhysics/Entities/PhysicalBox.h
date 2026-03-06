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
	* PhysicalObject with a box shape
	*/
	class PhysicalBox : public PhysicalObject
	{
	public:
		/**
		* PhysicalBox constructor (Internal transform management)
		* @param p_radius
		* @param p_height
		*/
		PhysicalBox(const OvMaths::FVector3& p_size = {0.5f, 0.5f, 0.5f});

		/**
		* PhysicalBox constructor (External transform management)
		* @param p_transform
		* @param p_radius
		* @param p_height
		*/
		PhysicalBox(OvMaths::FTransform& p_transform, const OvMaths::FVector3& p_size = { 0.5f, 0.5f, 0.5f });

		/**
		* Defines the size of the physical box
		* @param p_size
		*/
		void SetSize(const OvMaths::FVector3& p_size);

		/**
		* Returns the size of the physical box
		*/
		OvMaths::FVector3 GetSize() const;

	private:
		void CreateCollisionShape(const OvMaths::FVector3& p_size);
		void RecreateCollisionShape(const OvMaths::FVector3& p_size);
		virtual void SetLocalScaling(const OvMaths::FVector3& p_scaling) override;

	private:
		OvMaths::FVector3 m_size;
	};
}