/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <limits>

#include <optional>
#include <string>
#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CSkinnedMeshRenderer.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvMaths/FMatrix3.h>
#include <OvMaths/FTransform.h>
#include <OvUI/Plugins/DataDispatcher.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Widgets/Texts/Text.h>

namespace
{
	float WrapTime(float p_value, float p_duration)
	{
		if (p_duration <= 0.0f)
		{
			return 0.0f;
		}

		const auto wrapped = std::fmod(p_value, p_duration);
		return wrapped < 0.0f ? wrapped + p_duration : wrapped;
	}

	template<typename T, typename TLerp>
	T SampleKeys(
		const std::vector<OvRendering::Animation::Keyframe<T>>& p_keys,
		float p_time,
		float p_duration,
		const T& p_defaultValue,
		bool p_looping,
		TLerp p_lerp
	)
	{
		if (p_keys.empty())
		{
			return p_defaultValue;
		}

		if (p_keys.size() == 1)
		{
			return p_keys.front().value;
		}

		if (!p_looping)
		{
			if (p_time <= p_keys.front().time) return p_keys.front().value;
			if (p_time >= p_keys.back().time)  return p_keys.back().value;
		}

		const auto nextIt = std::upper_bound(
			p_keys.begin(),
			p_keys.end(),
			p_time,
			[](float p_lhs, const auto& p_rhs) { return p_lhs < p_rhs.time; }
		);

		const auto interpolate = [&](const auto& p_prev, const auto& p_next, float p_segmentDuration)
		{
			if (p_segmentDuration <= std::numeric_limits<float>::epsilon())
			{
				return p_prev.value;
			}

			const float alpha = std::clamp((p_time - p_prev.time) / p_segmentDuration, 0.0f, 1.0f);
			return p_lerp(p_prev.value, p_next.value, alpha);
		};

		if (nextIt == p_keys.end())
		{
			if (!p_looping)
			{
				return p_keys.back().value;
			}

			const auto& prev = p_keys.back();
			const auto& next = p_keys.front();
			return interpolate(prev, next, (p_duration - prev.time) + next.time);
		}

		if (nextIt == p_keys.begin())
		{
			return nextIt->value;
		}

		const auto& prev = *std::prev(nextIt);
		const auto& next = *nextIt;
		return interpolate(prev, next, next.time - prev.time);
	}

	void DecomposeLocalTransform(
		const OvMaths::FMatrix4& p_matrix,
		OvMaths::FVector3& p_position,
		OvMaths::FQuaternion& p_rotation,
		OvMaths::FVector3& p_scale
	)
	{
		p_position.x = p_matrix.data[3];
		p_position.y = p_matrix.data[7];
		p_position.z = p_matrix.data[11];

		OvMaths::FVector3 columns[3] =
		{
			{ p_matrix.data[0], p_matrix.data[4], p_matrix.data[8] },
			{ p_matrix.data[1], p_matrix.data[5], p_matrix.data[9] },
			{ p_matrix.data[2], p_matrix.data[6], p_matrix.data[10] }
		};

		p_scale.x = OvMaths::FVector3::Length(columns[0]);
		p_scale.y = OvMaths::FVector3::Length(columns[1]);
		p_scale.z = OvMaths::FVector3::Length(columns[2]);

		if (p_scale.x > 0.0f)
		{
			columns[0] /= p_scale.x;
		}

		if (p_scale.y > 0.0f)
		{
			columns[1] /= p_scale.y;
		}

		if (p_scale.z > 0.0f)
		{
			columns[2] /= p_scale.z;
		}

		const OvMaths::FMatrix3 rotationMatrix(
			columns[0].x, columns[1].x, columns[2].x,
			columns[0].y, columns[1].y, columns[2].y,
			columns[0].z, columns[1].z, columns[2].z
		);

		p_rotation = OvMaths::FQuaternion(rotationMatrix);
	}
}

OvCore::ECS::Components::CSkinnedMeshRenderer::CSkinnedMeshRenderer(ECS::Actor& p_owner) :
	AComponent(p_owner)
{
	NotifyModelChanged();
}

std::string OvCore::ECS::Components::CSkinnedMeshRenderer::GetName()
{
	return "Skinned Mesh Renderer";
}

std::string OvCore::ECS::Components::CSkinnedMeshRenderer::GetTypeName()
{
	return std::string{ ComponentTraits<CSkinnedMeshRenderer>::Name };
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::NotifyModelChanged()
{
	m_model = nullptr;
	SyncWithModel();
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::HasSkinningData() const
{
	return HasCompatibleModel() && !m_boneMatrices.empty() && (m_animationIndex.has_value() || m_manualPoseOverride);
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::Play()
{
	m_playing = true;
	m_poseEvaluationAccumulator = 0.0f;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::Pause()
{
	m_playing = false;
	m_poseEvaluationAccumulator = 0.0f;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::Stop()
{
	m_playing = false;
	m_currentTimeTicks = 0.0f;
	m_poseEvaluationAccumulator = 0.0f;
	m_manualPoseOverride = false;
	EvaluatePose();
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::IsPlaying() const
{
	return m_playing;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::SetLooping(bool p_value)
{
	m_looping = p_value;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::IsLooping() const
{
	return m_looping;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::SetPlaybackSpeed(float p_value)
{
	m_playbackSpeed = p_value;
}

float OvCore::ECS::Components::CSkinnedMeshRenderer::GetPlaybackSpeed() const
{
	return m_playbackSpeed;
}

float OvCore::ECS::Components::CSkinnedMeshRenderer::GetMeshBoundsScale() const
{
	return m_meshBoundsScale;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::SetMeshBoundsScale(float p_scale)
{
	m_meshBoundsScale = std::max(1.0f, p_scale);
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::SetTime(float p_timeSeconds)
{
	if (!HasCompatibleModel() || !m_animationIndex.has_value())
	{
		return;
	}

	const auto& animation = m_model->GetAnimations().at(*m_animationIndex);
	const float ticksPerSecond = animation.GetEffectiveTicksPerSecond();

	m_currentTimeTicks = p_timeSeconds * ticksPerSecond;
	if (m_looping)
	{
		m_currentTimeTicks = WrapTime(m_currentTimeTicks, animation.duration);
	}
	else
	{
		m_currentTimeTicks = std::clamp(m_currentTimeTicks, 0.0f, animation.duration);
	}

	m_poseEvaluationAccumulator = 0.0f;
	EvaluatePose();
}

float OvCore::ECS::Components::CSkinnedMeshRenderer::GetTime() const
{
	if (!HasCompatibleModel() || !m_animationIndex.has_value())
	{
		return 0.0f;
	}

	const auto& animation = m_model->GetAnimations().at(*m_animationIndex);
	const float ticksPerSecond = animation.GetEffectiveTicksPerSecond();
	return m_currentTimeTicks / ticksPerSecond;
}

uint32_t OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationCount() const
{
	return static_cast<uint32_t>(m_animationNames.size());
}

std::optional<std::string> OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationName(uint32_t p_index) const
{
	if (p_index < m_animationNames.size())
	{
		return m_animationNames[p_index];
	}

	return std::nullopt;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::SetAnimation(std::optional<uint32_t> p_index)
{
	if (!p_index.has_value())
	{
		m_animationIndex = std::nullopt;
		m_currentTimeTicks = 0.0f;
		m_poseEvaluationAccumulator = 0.0f;
		m_manualPoseOverride = false;
		EvaluatePose();
		return true;
	}

	if (!HasCompatibleModel() || *p_index >= m_model->GetAnimations().size())
	{
		return false;
	}

	m_animationIndex = p_index;
	m_currentTimeTicks = 0.0f;
	m_poseEvaluationAccumulator = 0.0f;
	m_manualPoseOverride = false;
	EvaluatePose();
	return true;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::SetAnimation(const std::string& p_name)
{
	if (!HasCompatibleModel())
	{
		return false;
	}

	const auto& animations = m_model->GetAnimations();

	const auto found = std::find_if(animations.begin(), animations.end(), [&p_name](const auto& p_animation)
	{
		return p_animation.name == p_name;
	});

	if (found == animations.end())
	{
		return false;
	}

	return SetAnimation(static_cast<uint32_t>(std::distance(animations.begin(), found)));
}

std::optional<uint32_t> OvCore::ECS::Components::CSkinnedMeshRenderer::GetActiveAnimationIndex() const
{
	return m_animationIndex;
}

std::optional<std::string> OvCore::ECS::Components::CSkinnedMeshRenderer::GetActiveAnimationName() const
{
	if (!m_animationIndex.has_value())
	{
		return std::nullopt;
	}

	return GetAnimationName(m_animationIndex.value());
}

uint32_t OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneCount() const
{
	if (!HasCompatibleModel())
	{
		return 0;
	}

	return static_cast<uint32_t>(m_model->GetSkeleton().value().bones.size());
}

std::optional<std::string> OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneName(uint32_t p_index) const
{
	if (!HasCompatibleModel())
	{
		return std::nullopt;
	}

	const auto& bones = m_model->GetSkeleton().value().bones;
	if (p_index >= bones.size())
	{
		return std::nullopt;
	}

	return bones[p_index].name;
}

std::optional<uint32_t> OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneIndex(const std::string& p_name) const
{
	if (!HasCompatibleModel())
	{
		return std::nullopt;
	}

	return m_model->GetSkeleton().value().FindBoneIndex(p_name);
}

std::optional<OvMaths::FVector3> OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneLocalPosition(uint32_t p_boneIndex) const
{
	const auto nodeIndex = GetNodeIndexFromBoneIndex(p_boneIndex);
	if (!nodeIndex.has_value())
	{
		return std::nullopt;
	}

	OvMaths::FVector3 position;
	OvMaths::FQuaternion rotation;
	OvMaths::FVector3 scale;
	DecomposeLocalTransform(m_localPose[*nodeIndex], position, rotation, scale);
	return position;
}

std::optional<OvMaths::FQuaternion> OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneLocalRotation(uint32_t p_boneIndex) const
{
	const auto nodeIndex = GetNodeIndexFromBoneIndex(p_boneIndex);
	if (!nodeIndex.has_value())
	{
		return std::nullopt;
	}

	OvMaths::FVector3 position;
	OvMaths::FQuaternion rotation;
	OvMaths::FVector3 scale;
	DecomposeLocalTransform(m_localPose[*nodeIndex], position, rotation, scale);
	return rotation;
}

std::optional<OvMaths::FVector3> OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneLocalScale(uint32_t p_boneIndex) const
{
	const auto nodeIndex = GetNodeIndexFromBoneIndex(p_boneIndex);
	if (!nodeIndex.has_value())
	{
		return std::nullopt;
	}

	OvMaths::FVector3 position;
	OvMaths::FQuaternion rotation;
	OvMaths::FVector3 scale;
	DecomposeLocalTransform(m_localPose[*nodeIndex], position, rotation, scale);
	return scale;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::SetBoneLocalPosition(uint32_t p_boneIndex, const OvMaths::FVector3& p_position)
{
	const auto nodeIndex = GetNodeIndexFromBoneIndex(p_boneIndex);
	if (!nodeIndex.has_value())
	{
		return false;
	}

	OvMaths::FVector3 currentPosition;
	OvMaths::FQuaternion currentRotation;
	OvMaths::FVector3 currentScale;
	DecomposeLocalTransform(m_localPose[*nodeIndex], currentPosition, currentRotation, currentScale);

	const OvMaths::FTransform transform(p_position, currentRotation, currentScale);
	m_localPose[*nodeIndex] = transform.GetLocalMatrix();
	m_manualPoseOverride = true;
	RecomputeBoneMatricesFromLocalPose();
	return true;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::SetBoneLocalRotation(uint32_t p_boneIndex, const OvMaths::FQuaternion& p_rotation)
{
	const auto nodeIndex = GetNodeIndexFromBoneIndex(p_boneIndex);
	if (!nodeIndex.has_value())
	{
		return false;
	}

	OvMaths::FVector3 currentPosition;
	OvMaths::FQuaternion currentRotation;
	OvMaths::FVector3 currentScale;
	DecomposeLocalTransform(m_localPose[*nodeIndex], currentPosition, currentRotation, currentScale);

	const OvMaths::FTransform transform(currentPosition, p_rotation, currentScale);
	m_localPose[*nodeIndex] = transform.GetLocalMatrix();
	m_manualPoseOverride = true;
	RecomputeBoneMatricesFromLocalPose();
	return true;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::SetBoneLocalScale(uint32_t p_boneIndex, const OvMaths::FVector3& p_scale)
{
	const auto nodeIndex = GetNodeIndexFromBoneIndex(p_boneIndex);
	if (!nodeIndex.has_value())
	{
		return false;
	}

	OvMaths::FVector3 currentPosition;
	OvMaths::FQuaternion currentRotation;
	OvMaths::FVector3 currentScale;
	DecomposeLocalTransform(m_localPose[*nodeIndex], currentPosition, currentRotation, currentScale);

	const OvMaths::FTransform transform(currentPosition, currentRotation, p_scale);
	m_localPose[*nodeIndex] = transform.GetLocalMatrix();
	m_manualPoseOverride = true;
	RecomputeBoneMatricesFromLocalPose();
	return true;
}

const std::vector<OvMaths::FMatrix4>& OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneMatricesTransposed() const
{
	return m_boneMatricesTransposed;
}

uint64_t OvCore::ECS::Components::CSkinnedMeshRenderer::GetPoseVersion() const
{
	return m_poseVersion;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnUpdate(float p_deltaTime)
{
	if (!owner.IsActive())
	{
		return;
	}

	SyncWithModel();

	if (!HasCompatibleModel() || !m_playing)
	{
		return;
	}

	const float previousTimeTicks = m_currentTimeTicks;
	const bool wasPlaying = m_playing;

	UpdatePlayback(p_deltaTime);

	const bool timeChanged = std::abs(m_currentTimeTicks - previousTimeTicks) > std::numeric_limits<float>::epsilon();
	const bool playbackStateChanged = wasPlaying != m_playing;

	if (timeChanged || playbackStateChanged)
	{
		const float clampedPoseEvaluationRate = std::max(0.0f, m_poseEvaluationRate);
		const bool hasRateLimit = clampedPoseEvaluationRate > std::numeric_limits<float>::epsilon();

		if (hasRateLimit)
		{
			m_poseEvaluationAccumulator += p_deltaTime;
			const float updatePeriod = 1.0f / clampedPoseEvaluationRate;
			if (m_poseEvaluationAccumulator < updatePeriod && !playbackStateChanged)
			{
				return;
			}

			m_poseEvaluationAccumulator = std::fmod(m_poseEvaluationAccumulator, updatePeriod);
		}
		else
		{
			m_poseEvaluationAccumulator = 0.0f;
		}

		EvaluatePose();
	}
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "playing", m_playing);
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "looping", m_looping);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "playback_speed", m_playbackSpeed);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "mesh_bounds_scale", m_meshBoundsScale);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "pose_eval_rate", m_poseEvaluationRate);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "time_ticks", m_currentTimeTicks);
	OvCore::Helpers::Serializer::SerializeString(p_doc, p_node, "animation", GetActiveAnimationName().value_or(std::string{}));
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "playing", m_playing);
	OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "looping", m_looping);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "playback_speed", m_playbackSpeed);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "mesh_bounds_scale", m_meshBoundsScale);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "pose_eval_rate", m_poseEvaluationRate);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "time_ticks", m_currentTimeTicks);
	OvCore::Helpers::Serializer::DeserializeString(p_doc, p_node, "animation", m_deserializedAnimationName);
	SetMeshBoundsScale(m_meshBoundsScale);
	m_poseEvaluationRate = std::max(0.0f, m_poseEvaluationRate);
	m_poseEvaluationAccumulator = 0.0f;

	NotifyModelChanged();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	SyncWithModel();

	using namespace OvCore::Helpers;

	GUIDrawer::DrawBoolean(p_root, "Playing", m_playing);
	GUIDrawer::DrawBoolean(p_root, "Looping", m_looping);
	GUIDrawer::DrawScalar<float>(p_root, "Playback Speed", m_playbackSpeed, 0.01f, -10.0f, 10.0f);
	GUIDrawer::DrawScalar<float>(p_root, "Mesh Bounds Scale", m_meshBoundsScale, 0.05f, 1.0f, 10.0f);
	GUIDrawer::DrawScalar<float>(p_root, "Pose Eval Rate", m_poseEvaluationRate, 1.0f, 0.0f, 240.0f);
	m_poseEvaluationRate = std::max(0.0f, m_poseEvaluationRate);
	GUIDrawer::DrawScalar<float>(
		p_root,
		"Time (Seconds)",
		[this]() { return GetTime(); },
		[this](float p_value) { SetTime(p_value); },
		0.01f,
		0.0f,
		std::max(GetAnimationDurationSeconds(), 3600.0f)
	);

	GUIDrawer::CreateTitle(p_root, "Active Animation");
	const int currentAnimIndex = GetActiveAnimationIndex().has_value() ? static_cast<int>(*GetActiveAnimationIndex()) : -1;
	auto& animationChoice = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(currentAnimIndex);

	auto& animDispatcher = animationChoice.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	animDispatcher.RegisterGatherer([this, &animationChoice]
	{
		animationChoice.choices.clear();
		animationChoice.choices.emplace(-1, "<None>");

		for (size_t i = 0; i < m_animationNames.size(); ++i)
		{
			animationChoice.choices.emplace(static_cast<int>(i), m_animationNames[i]);
		}

		return GetActiveAnimationIndex().has_value() ? static_cast<int>(*GetActiveAnimationIndex()) : -1;
	});
	animDispatcher.RegisterProvider([this](int p_choice)
	{
		SetAnimation(p_choice >= 0 ? std::make_optional(static_cast<uint32_t>(p_choice)) : std::nullopt);
	});

	if (!HasCompatibleModel())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::Text>("No skinned model assigned");
	}
	else if (m_animationNames.empty())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::Text>("Model has no animation clips");
	}
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::HasCompatibleModel() const
{
	return m_model && m_model->IsSkinned() && m_model->GetSkeleton().has_value();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::SyncWithModel()
{
	const auto modelRenderer = owner.GetComponent<CModelRenderer>();
	const auto model = modelRenderer ? modelRenderer->GetModel() : nullptr;

	if (m_model == model)
	{
		return;
	}

	m_model = model;
	RebuildRuntimeData();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::RebuildRuntimeData()
{
	const float preservedTimeTicks = m_currentTimeTicks;
	const std::optional<uint32_t> preservedAnimationIndex = m_animationIndex;
	const std::string requestedAnimationName = m_deserializedAnimationName;

	m_animationNames.clear();
	m_localPose.clear();
	m_globalPose.clear();
	m_boneMatrices.clear();
	m_boneMatricesTransposed.clear();
	m_animationIndex = std::nullopt;
	m_currentTimeTicks = preservedTimeTicks;
	m_poseEvaluationAccumulator = 0.0f;
	m_manualPoseOverride = false;

	if (!HasCompatibleModel())
	{
		return;
	}

	const auto& skeleton = m_model->GetSkeleton().value();
	const auto& animations = m_model->GetAnimations();

	m_localPose.resize(skeleton.nodes.size(), OvMaths::FMatrix4::Identity);
	m_globalPose.resize(skeleton.nodes.size(), OvMaths::FMatrix4::Identity);
	m_boneMatrices.resize(skeleton.bones.size(), OvMaths::FMatrix4::Identity);
	m_boneMatricesTransposed.resize(skeleton.bones.size(), OvMaths::FMatrix4::Identity);

	m_animationNames.reserve(animations.size());
	for (const auto& animation : animations)
	{
		m_animationNames.push_back(animation.name);
	}

	if (!animations.empty())
	{
		if (!requestedAnimationName.empty())
		{
			const auto found = std::find(m_animationNames.begin(), m_animationNames.end(), requestedAnimationName);
			m_animationIndex = found != m_animationNames.end() ?
				std::optional<uint32_t>{ static_cast<uint32_t>(std::distance(m_animationNames.begin(), found)) } :
				std::optional<uint32_t>{ 0 };
		}
		else if (preservedAnimationIndex.has_value() && *preservedAnimationIndex < animations.size())
		{
			m_animationIndex = *preservedAnimationIndex;
		}
	}

	if (m_animationIndex.has_value() && *m_animationIndex < animations.size())
	{
		const auto& animation = animations.at(*m_animationIndex);
		if (m_looping)
		{
			m_currentTimeTicks = WrapTime(m_currentTimeTicks, animation.duration);
		}
		else
		{
			m_currentTimeTicks = std::clamp(m_currentTimeTicks, 0.0f, animation.duration);
		}
	}
	else
	{
		m_currentTimeTicks = 0.0f;
	}

	m_deserializedAnimationName.clear();
	EvaluatePose();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::EvaluatePose()
{
	if (!HasCompatibleModel())
	{
		return;
	}

	const auto& skeleton = m_model->GetSkeleton().value();

	for (size_t nodeIndex = 0; nodeIndex < skeleton.nodes.size(); ++nodeIndex)
	{
		m_localPose[nodeIndex] = skeleton.nodes[nodeIndex].localBindTransform;
	}

	if (m_animationIndex.has_value() && *m_animationIndex < m_model->GetAnimations().size())
	{
		const auto& animation = m_model->GetAnimations().at(*m_animationIndex);
		const float duration = std::max(animation.duration, 0.0f);
		const float sampleTime =
			duration > 0.0f ?
			(m_looping ? WrapTime(m_currentTimeTicks, duration) : std::clamp(m_currentTimeTicks, 0.0f, duration)) :
			0.0f;

		for (const auto& track : animation.tracks)
		{
			if (track.nodeIndex >= m_localPose.size())
			{
				continue;
			}

			const auto& node = skeleton.nodes[track.nodeIndex];

			const OvMaths::FVector3 sampledPosition = SampleKeys(
				track.positionKeys,
				sampleTime,
				duration,
				node.bindPosition,
				m_looping,
				[](const auto& p_a, const auto& p_b, float p_alpha) { return OvMaths::FVector3::Lerp(p_a, p_b, p_alpha); }
			);

			const OvMaths::FQuaternion sampledRotation = SampleKeys(
				track.rotationKeys,
				sampleTime,
				duration,
				node.bindRotation,
				m_looping,
				[](const auto& p_a, const auto& p_b, float p_alpha) { return OvMaths::FQuaternion::Slerp(p_a, p_b, p_alpha); }
			);

			const OvMaths::FVector3 sampledScale = SampleKeys(
				track.scaleKeys,
				sampleTime,
				duration,
				node.bindScale,
				m_looping,
				[](const auto& p_a, const auto& p_b, float p_alpha) { return OvMaths::FVector3::Lerp(p_a, p_b, p_alpha); }
			);

			const OvMaths::FTransform sampled(sampledPosition, sampledRotation, sampledScale);
			m_localPose[track.nodeIndex] = sampled.GetLocalMatrix();
		}
	}

	m_manualPoseOverride = false;
	RecomputeBoneMatricesFromLocalPose();
}

std::optional<uint32_t> OvCore::ECS::Components::CSkinnedMeshRenderer::GetNodeIndexFromBoneIndex(uint32_t p_boneIndex) const
{
	if (!HasCompatibleModel())
	{
		return std::nullopt;
	}

	const auto& skeleton = m_model->GetSkeleton().value();
	if (p_boneIndex >= skeleton.bones.size())
	{
		return std::nullopt;
	}

	const uint32_t nodeIndex = skeleton.bones[p_boneIndex].nodeIndex;
	if (nodeIndex >= m_localPose.size())
	{
		return std::nullopt;
	}

	return nodeIndex;
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::RecomputeBoneMatricesFromLocalPose()
{
	if (!HasCompatibleModel())
	{
		return;
	}

	const auto& skeleton = m_model->GetSkeleton().value();
	if (
		m_localPose.size() != skeleton.nodes.size() ||
		m_globalPose.size() != skeleton.nodes.size() ||
		m_boneMatrices.size() != skeleton.bones.size() ||
		m_boneMatricesTransposed.size() != skeleton.bones.size()
	)
	{
		return;
	}

	for (size_t nodeIndex = 0; nodeIndex < skeleton.nodes.size(); ++nodeIndex)
	{
		const auto parentIndex = skeleton.nodes[nodeIndex].parentIndex;
		m_globalPose[nodeIndex] =
			parentIndex >= 0 ?
			m_globalPose[static_cast<size_t>(parentIndex)] * m_localPose[nodeIndex] :
			m_localPose[nodeIndex];
	}

	for (size_t boneIndex = 0; boneIndex < skeleton.bones.size(); ++boneIndex)
	{
		const auto& bone = skeleton.bones[boneIndex];
		m_boneMatrices[boneIndex] =
			bone.nodeIndex < m_globalPose.size() ?
			m_globalPose[bone.nodeIndex] * bone.offsetMatrix :
			OvMaths::FMatrix4::Identity;

		m_boneMatricesTransposed[boneIndex] = OvMaths::FMatrix4::Transpose(m_boneMatrices[boneIndex]);
	}

	++m_poseVersion;
}

float OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationDurationSeconds() const
{
	if (!HasCompatibleModel() || !m_animationIndex.has_value())
	{
		return 0.0f;
	}

	const auto& animation = m_model->GetAnimations().at(*m_animationIndex);
	return animation.GetDurationSeconds();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::UpdatePlayback(float p_deltaTime)
{
	if (!HasCompatibleModel() || !m_animationIndex.has_value())
	{
		return;
	}

	const auto& animation = m_model->GetAnimations().at(*m_animationIndex);
	if (animation.duration <= 0.0f)
	{
		return;
	}

	if (std::abs(m_playbackSpeed) <= std::numeric_limits<float>::epsilon())
	{
		return;
	}

	const float ticksPerSecond = animation.GetEffectiveTicksPerSecond();
	m_currentTimeTicks += p_deltaTime * ticksPerSecond * m_playbackSpeed;

	if (m_looping)
	{
		m_currentTimeTicks = WrapTime(m_currentTimeTicks, animation.duration);
	}
	else
	{
		const float clamped = std::clamp(m_currentTimeTicks, 0.0f, animation.duration);
		const bool reachedStart = clamped <= 0.0f && m_playbackSpeed < 0.0f;
		const bool reachedEnd = clamped >= animation.duration && m_playbackSpeed > 0.0f;
		m_currentTimeTicks = clamped;
		if (reachedStart || reachedEnd)
		{
			m_playing = false;
		}
	}
}
