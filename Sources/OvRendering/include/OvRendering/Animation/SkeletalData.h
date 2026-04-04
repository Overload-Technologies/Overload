/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <OvMaths/FMatrix4.h>
#include <OvMaths/FQuaternion.h>
#include <OvMaths/FVector3.h>

namespace OvRendering::Animation
{
	constexpr uint8_t kMaxBonesPerVertex = 4;

	struct SkeletonNode
	{
		std::string name;
		int32_t parentIndex = -1;
		int32_t boneIndex = -1;
		OvMaths::FMatrix4 localBindTransform = OvMaths::FMatrix4::Identity;

		// Decomposed from localBindTransform — used as default values when a track has no keys
		OvMaths::FVector3 bindPosition;
		OvMaths::FQuaternion bindRotation;
		OvMaths::FVector3 bindScale = OvMaths::FVector3(1.0f, 1.0f, 1.0f);
	};

	struct Bone
	{
		std::string name;
		uint32_t nodeIndex = 0;
		OvMaths::FMatrix4 offsetMatrix = OvMaths::FMatrix4::Identity;
	};

	struct Skeleton
	{
		std::vector<SkeletonNode> nodes;
		std::vector<Bone> bones;
		std::unordered_map<std::string, uint32_t> nodeByName;
		std::unordered_map<std::string, uint32_t> boneByName;

		bool IsValid() const
		{
			return !nodes.empty() && !bones.empty();
		}

		std::optional<uint32_t> FindNodeIndex(std::string_view p_name) const
		{
			if (auto found = nodeByName.find(std::string{ p_name }); found != nodeByName.end())
			{
				return found->second;
			}

			return std::nullopt;
		}

		std::optional<uint32_t> FindBoneIndex(std::string_view p_name) const
		{
			if (auto found = boneByName.find(std::string{ p_name }); found != boneByName.end())
			{
				return found->second;
			}

			return std::nullopt;
		}
	};

	template<typename T>
	struct Keyframe
	{
		float time = 0.0f; // Ticks
		T value{};
	};

	struct NodeAnimationTrack
	{
		uint32_t nodeIndex = 0;
		std::vector<Keyframe<OvMaths::FVector3>> positionKeys;
		std::vector<Keyframe<OvMaths::FQuaternion>> rotationKeys;
		std::vector<Keyframe<OvMaths::FVector3>> scaleKeys;
	};

	struct SkeletalAnimation
	{
		std::string name;
		float duration = 0.0f; // Ticks
		float ticksPerSecond = 25.0f;
		std::vector<NodeAnimationTrack> tracks;
		std::unordered_map<uint32_t, uint32_t> trackByNodeIndex;

		bool IsValid() const
		{
			return duration > 0.0f && !tracks.empty();
		}

		float GetDurationSeconds() const
		{
			return ticksPerSecond > 0.0f ? duration / ticksPerSecond : 0.0f;
		}

		float GetEffectiveTicksPerSecond() const
		{
			return ticksPerSecond > 0.0f ? ticksPerSecond : 25.0f;
		}

		const NodeAnimationTrack* FindTrack(uint32_t p_nodeIndex) const
		{
			if (auto found = trackByNodeIndex.find(p_nodeIndex); found != trackByNodeIndex.end())
			{
				return &tracks.at(found->second);
			}

			return nullptr;
		}
	};
}
