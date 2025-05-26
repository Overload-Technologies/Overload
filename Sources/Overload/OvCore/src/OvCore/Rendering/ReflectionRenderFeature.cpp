/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tracy/Tracy.hpp>

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/ReflectionRenderFeature.h>
#include <OvDebug/Logger.h>
#include <OvRendering/Features/LightingRenderFeature.h>

namespace
{
	bool IsAffectedByReflectionProbe(
		const OvMaths::FMatrix4& p_modelMatrix,
		const OvRendering::Geometry::BoundingSphere& p_bounds,
		const OvCore::ECS::Components::CReflectionProbe& p_probe
	)
	{
		if (p_probe.GetInfluencePolicy() == OvCore::ECS::Components::CReflectionProbe::EInfluencePolicy::GLOBAL)
		{
			return true;
		}

		// Transform the bounding sphere to world space using the model matrix
		const auto worldSphereCenter = p_modelMatrix * OvMaths::FVector4(p_bounds.position, 1.0f);

		// Calculate the world space radius by applying the model matrix scale
		// Extract the scale from the model matrix (assuming uniform scaling for simplicity)
		const auto scaleX = OvMaths::FVector3::Length({ p_modelMatrix.data[0], p_modelMatrix.data[1], p_modelMatrix.data[2] });
		const auto scaleY = OvMaths::FVector3::Length({ p_modelMatrix.data[4], p_modelMatrix.data[5], p_modelMatrix.data[6] });
		const auto scaleZ = OvMaths::FVector3::Length({ p_modelMatrix.data[8], p_modelMatrix.data[9], p_modelMatrix.data[10] });
		const auto maxScale = std::max({ scaleX, scaleY, scaleZ });
		const auto worldSphereRadius = p_bounds.radius * maxScale;

		// Get the probe's transform to construct the OBB
		const auto& probeTransform = p_probe.owner.transform;
		// Get the probe's influence position
		const auto& probeInfluencePosition =
			probeTransform.GetWorldPosition() +
			p_probe.GetInfluenceOffset();
		// Get the rotation matrix from the probe's transform
		const auto probeRotation = probeTransform.GetWorldRotation();

		// Convert sphere center to probe's local space
		const auto sphereCenterLocal = OvMaths::FVector3{
			worldSphereCenter.x,
			worldSphereCenter.y,
			worldSphereCenter.z
		} - probeInfluencePosition;

		// Rotate the sphere center into the OBB's local coordinate system
		const auto rotationMatrix = OvMaths::FMatrix4::Rotation(probeRotation);
		const auto sphereCenterOBBLocal = OvMaths::FMatrix4::Transpose(rotationMatrix) * sphereCenterLocal;

		// Half extents of the OBB (the size is already expressed as half extents)
		const auto halfExtents = p_probe.GetInfluenceSize();

		// Find the closest point on the AABB (in OBB local space) to the sphere center
		auto closestPoint = sphereCenterOBBLocal;

		// Clamp to AABB bounds
		closestPoint.x = std::max(-halfExtents.x, std::min(halfExtents.x, closestPoint.x));
		closestPoint.y = std::max(-halfExtents.y, std::min(halfExtents.y, closestPoint.y));
		closestPoint.z = std::max(-halfExtents.z, std::min(halfExtents.z, closestPoint.z));

		// Calculate distance squared from sphere center to closest point on OBB
		const auto diff = sphereCenterOBBLocal - closestPoint;
		const auto distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		// Check if distance is less than sphere radius
		return distanceSquared <= (worldSphereRadius * worldSphereRadius);
	}

	// The best suited probe will always be local over global, and when multiple probes
	// are found, the closest one to the drawable will be selected.
	OvTools::Utils::OptRef<const OvCore::ECS::Components::CReflectionProbe> FindBestReflectionProbe(
		const OvRendering::Entities::Drawable& p_drawable,
		const OvCore::Rendering::EngineDrawableDescriptor& p_engineDrawableDesc,
		std::span<const std::reference_wrapper<OvCore::ECS::Components::CReflectionProbe>> p_probes)
	{
		// Stores the best local and global probes found so far.
		struct {
			OvTools::Utils::OptRef<const OvCore::ECS::Components::CReflectionProbe> probe;
			float distance = std::numeric_limits<float>::max();
		} bestLocal, bestGlobal;

		// Extract the drawable's world position from the model matrix
		const auto& modelMatrix = p_engineDrawableDesc.modelMatrix;
		const auto drawableWorldPosition = OvMaths::FVector3{
			modelMatrix.data[3], // Translation X
			modelMatrix.data[7], // Translation Y
			modelMatrix.data[11]  // Translation Z
		};

		// Process all probes to find influencing ones
		for (auto& probeRef : p_probes)
		{
			const auto& probe = probeRef.get();

			const auto& probeTransform = probe.owner.transform;
			const auto probeWorldPosition = probeTransform.GetWorldPosition() + probe.GetInfluenceOffset();
			const auto distance = OvMaths::FVector3::Distance(drawableWorldPosition, probeWorldPosition);

			const bool isLocal = probe.GetInfluencePolicy() == OvCore::ECS::Components::CReflectionProbe::EInfluencePolicy::LOCAL;
			const bool isGlobal = !isLocal;

			auto& currentBest = isLocal ? bestLocal : bestGlobal;

			// Skip this probe if it's further than the best found so far.
			if (distance > currentBest.distance)
			{
				continue;
			}

			// Skip probes that don't affect this drawable
			if(!IsAffectedByReflectionProbe(
				p_engineDrawableDesc.modelMatrix,
				p_drawable.mesh->GetBoundingSphere(),
				probe))
			{
				continue;
			}

			// If we made it here, this probe is a candidate for the best probe.
			currentBest = { probe, distance };
		}

		return bestLocal.probe ? bestLocal.probe : bestGlobal.probe;
	}
}

OvCore::Rendering::ReflectionRenderFeature::ReflectionRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer) :
	ARenderFeature(p_renderer)
{
}

void OvCore::Rendering::ReflectionRenderFeature::OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{
	ZoneScoped;

	auto& material = p_drawable.material.value();

	// TODO: Check if material is set to receive reflections
	if (!material.HasProperty("_ReflectionProbe"))
	{
		return;
	}

	OVASSERT(m_renderer.HasDescriptor<ReflectionRenderFeature::ReflectionDescriptor>(), "Cannot find ReflectionDescriptor attached to this renderer");
	OVASSERT(p_drawable.HasDescriptor<OvCore::Rendering::EngineDrawableDescriptor>(), "Cannot find EngineDrawableDescriptor attached to this drawable");

	const auto& reflectionDescriptor = m_renderer.GetDescriptor<ReflectionRenderFeature::ReflectionDescriptor>();
	const auto& engineDrawableDesc = p_drawable.GetDescriptor<OvCore::Rendering::EngineDrawableDescriptor>();

	// Find the probe that is best suited for this drawable.
	auto targetProbe = FindBestReflectionProbe(
		p_drawable,
		engineDrawableDesc,
		reflectionDescriptor.reflectionProbes
	);

	material.SetProperty(
		"_ReflectionProbe",
		targetProbe ? targetProbe->_GetCubemap().get() : static_cast<OvRendering::HAL::TextureHandle*>(nullptr),
		true
	);

	if (targetProbe)
	{
		material.SetProperty("_ReflectionProbe", targetProbe->_GetCubemap().get(), true);

		if (material.HasProperty("_ReflectionProbeBoxProjection"))
		{
			const bool isGlobal = targetProbe->GetInfluencePolicy() == ECS::Components::CReflectionProbe::EInfluencePolicy::LOCAL;
			const bool boxProjection = isGlobal && targetProbe->IsBoxProjectionEnabled();
			material.SetProperty("_ReflectionProbeBoxProjection", boxProjection, true);

			if (boxProjection)
			{
				const auto& probePosition = targetProbe->owner.transform.GetWorldPosition();
				const auto& boxPosition = probePosition + targetProbe->GetInfluenceOffset();
				const auto& probeRotation = targetProbe->owner.transform.GetWorldRotation();
				const auto& probeRotationMatrix = OvMaths::FQuaternion::ToMatrix3(OvMaths::FQuaternion::Normalize(probeRotation));

				material.TrySetProperty("_ReflectionProbePosition", probePosition, true);
				material.TrySetProperty("_ReflectionProbeRotation", probeRotationMatrix, true);
				material.TrySetProperty("_ReflectionProbeBoxCenter", boxPosition, true);
				material.TrySetProperty("_ReflectionProbeBoxExtents", targetProbe->GetInfluenceSize() / 2.0f, true);
			}
		}
	}
}

void OvCore::Rendering::ReflectionRenderFeature::OnAfterDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{

}
