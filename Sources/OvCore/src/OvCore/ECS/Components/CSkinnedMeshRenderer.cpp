/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <limits>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CSkinnedMeshRenderer.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Widgets/Texts/Text.h>

namespace
{
	constexpr float kDefaultTicksPerSecond = 25.0f;

	void DecomposeTRS(
		const OvMaths::FMatrix4& p_matrix,
		OvMaths::FVector3& p_position,
		OvMaths::FQuaternion& p_rotation,
		OvMaths::FVector3& p_scale
	)
	{
		constexpr float kEpsilon = 1e-8f;

		p_position = {
			p_matrix.data[3],
			p_matrix.data[7],
			p_matrix.data[11]
		};

		OvMaths::FVector3 columns[3] = {
			{ p_matrix.data[0], p_matrix.data[4], p_matrix.data[8] },
			{ p_matrix.data[1], p_matrix.data[5], p_matrix.data[9] },
			{ p_matrix.data[2], p_matrix.data[6], p_matrix.data[10] }
		};

		p_scale.x = OvMaths::FVector3::Length(columns[0]);
		p_scale.y = OvMaths::FVector3::Length(columns[1]);
		p_scale.z = OvMaths::FVector3::Length(columns[2]);

		if (p_scale.x > kEpsilon) columns[0] /= p_scale.x;
		if (p_scale.y > kEpsilon) columns[1] /= p_scale.y;
		if (p_scale.z > kEpsilon) columns[2] /= p_scale.z;

		const float basisDeterminant = OvMaths::FVector3::Dot(
			OvMaths::FVector3::Cross(columns[0], columns[1]),
			columns[2]
		);

		if (basisDeterminant < 0.0f)
		{
			if (p_scale.x >= p_scale.y && p_scale.x >= p_scale.z)
			{
				p_scale.x = -p_scale.x;
				columns[0] = -columns[0];
			}
			else if (p_scale.y >= p_scale.x && p_scale.y >= p_scale.z)
			{
				p_scale.y = -p_scale.y;
				columns[1] = -columns[1];
			}
			else
			{
				p_scale.z = -p_scale.z;
				columns[2] = -columns[2];
			}
		}

		const OvMaths::FMatrix3 rotationMatrix{
			columns[0].x, columns[1].x, columns[2].x,
			columns[0].y, columns[1].y, columns[2].y,
			columns[0].z, columns[1].z, columns[2].z
		};

		p_rotation = OvMaths::FQuaternion::Normalize(OvMaths::FQuaternion(rotationMatrix));
	}

	OvMaths::FMatrix4 ComposeTRS(
		const OvMaths::FVector3& p_position,
		const OvMaths::FQuaternion& p_rotation,
		const OvMaths::FVector3& p_scale
	)
	{
		return
			OvMaths::FMatrix4::Translation(p_position) *
			OvMaths::FQuaternion::ToMatrix4(OvMaths::FQuaternion::Normalize(p_rotation)) *
			OvMaths::FMatrix4::Scaling(p_scale);
	}

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
		TLerp p_lerp,
		size_t* p_cursor = nullptr,
		bool p_useCursorHint = false
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

		const auto sampleWithSegment = [&](const auto& p_prev, const auto& p_next, float p_segmentDuration, float p_alphaBias = 0.0f)
		{
			if (p_segmentDuration <= std::numeric_limits<float>::epsilon())
			{
				return p_prev.value;
			}

			const float alpha = std::clamp(((p_time - p_prev.time) + p_alphaBias) / p_segmentDuration, 0.0f, 1.0f);
			return p_lerp(p_prev.value, p_next.value, alpha);
		};

		if (p_useCursorHint && p_cursor)
		{
			size_t cursor = std::min(*p_cursor, p_keys.size() - 1);
			while (cursor + 1 < p_keys.size() && p_time >= p_keys[cursor + 1].time)
			{
				++cursor;
			}

			*p_cursor = cursor;

			if (cursor + 1 < p_keys.size())
			{
				return sampleWithSegment(p_keys[cursor], p_keys[cursor + 1], p_keys[cursor + 1].time - p_keys[cursor].time);
			}

			if (!p_looping)
			{
				return p_keys.back().value;
			}

			const auto& prev = p_keys.back();
			const auto& next = p_keys.front();
			const float segmentDuration = (p_duration - prev.time) + next.time;
			return sampleWithSegment(prev, next, segmentDuration);
		}

		if (!p_looping)
		{
			if (p_time <= p_keys.front().time)
			{
				if (p_cursor)
				{
					*p_cursor = 0;
				}

				return p_keys.front().value;
			}

			if (p_time >= p_keys.back().time)
			{
				if (p_cursor)
				{
					*p_cursor = p_keys.size() - 1;
				}

				return p_keys.back().value;
			}
		}

		const auto nextIt = std::upper_bound(
			p_keys.begin(),
			p_keys.end(),
			p_time,
			[](float p_lhs, const auto& p_rhs) { return p_lhs < p_rhs.time; }
		);

		if (nextIt == p_keys.end())
		{
			if (!p_looping)
			{
				if (p_cursor)
				{
					*p_cursor = p_keys.size() - 1;
				}

				return p_keys.back().value;
			}

			const auto& prev = p_keys.back();
			const auto& next = p_keys.front();

			const float segmentDuration = (p_duration - prev.time) + next.time;
			if (p_cursor)
			{
				*p_cursor = p_keys.size() - 1;
			}

			return sampleWithSegment(prev, next, segmentDuration);
		}

		if (nextIt == p_keys.begin())
		{
			if (p_cursor)
			{
				*p_cursor = 0;
			}

			return nextIt->value;
		}

		const auto& prev = *std::prev(nextIt);
		const auto& next = *nextIt;
		if (p_cursor)
		{
			*p_cursor = static_cast<size_t>(std::distance(p_keys.begin(), std::prev(nextIt)));
		}

		return sampleWithSegment(prev, next, next.time - prev.time);
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

bool OvCore::ECS::Components::CSkinnedMeshRenderer::HasCompatibleModel() const
{
	return m_model && m_model->IsSkinned() && m_model->GetSkeleton().has_value();
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::HasSkinningData() const
{
	return HasCompatibleModel() && !m_boneMatrices.empty();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::SetEnabled(bool p_value)
{
	m_enabled = p_value;
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::IsEnabled() const
{
	return m_enabled;
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

void OvCore::ECS::Components::CSkinnedMeshRenderer::SetTime(float p_timeSeconds)
{
	if (!HasCompatibleModel() || m_animationIndex < 0)
	{
		return;
	}

	const auto& animation = m_model->GetAnimations().at(static_cast<size_t>(m_animationIndex));
	const float ticksPerSecond = animation.ticksPerSecond > 0.0f ? animation.ticksPerSecond : kDefaultTicksPerSecond;

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
	if (!HasCompatibleModel() || m_animationIndex < 0)
	{
		return 0.0f;
	}

	const auto& animation = m_model->GetAnimations().at(static_cast<size_t>(m_animationIndex));
	const float ticksPerSecond = animation.ticksPerSecond > 0.0f ? animation.ticksPerSecond : kDefaultTicksPerSecond;
	return ticksPerSecond > 0.0f ? m_currentTimeTicks / ticksPerSecond : 0.0f;
}

uint32_t OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationCount() const
{
	return static_cast<uint32_t>(m_animationNames.size());
}

std::string OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationName(uint32_t p_index) const
{
	if (p_index < m_animationNames.size())
	{
		return m_animationNames[p_index];
	}

	return {};
}

bool OvCore::ECS::Components::CSkinnedMeshRenderer::SetAnimation(uint32_t p_index)
{
	if (!HasCompatibleModel() || p_index >= m_model->GetAnimations().size())
	{
		return false;
	}

	m_animationIndex = static_cast<int32_t>(p_index);
	m_currentTimeTicks = 0.0f;
	m_poseEvaluationAccumulator = 0.0f;
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

int32_t OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationIndex() const
{
	return m_animationIndex;
}

std::string OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimation() const
{
	if (!HasCompatibleModel() || m_animationIndex < 0)
	{
		return {};
	}

	const auto animationIndex = static_cast<size_t>(m_animationIndex);
	return animationIndex < m_animationNames.size() ? m_animationNames[animationIndex] : std::string{};
}

const std::vector<OvMaths::FMatrix4>& OvCore::ECS::Components::CSkinnedMeshRenderer::GetBoneMatrices() const
{
	return m_boneMatrices;
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

	if (!m_enabled || !HasCompatibleModel())
	{
		return;
	}

	if (m_playing)
	{
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
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "enabled", m_enabled);
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "playing", m_playing);
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "looping", m_looping);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "playback_speed", m_playbackSpeed);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "pose_eval_rate", m_poseEvaluationRate);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "time_ticks", m_currentTimeTicks);
	OvCore::Helpers::Serializer::SerializeString(p_doc, p_node, "animation", GetAnimation());
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "enabled", m_enabled);
	OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "playing", m_playing);
	OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "looping", m_looping);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "playback_speed", m_playbackSpeed);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "pose_eval_rate", m_poseEvaluationRate);
	OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "time_ticks", m_currentTimeTicks);
	OvCore::Helpers::Serializer::DeserializeString(p_doc, p_node, "animation", m_deserializedAnimationName);
	m_poseEvaluationRate = std::max(0.0f, m_poseEvaluationRate);
	m_poseEvaluationAccumulator = 0.0f;

	NotifyModelChanged();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	SyncWithModel();

	using namespace OvCore::Helpers;

	GUIDrawer::DrawBoolean(p_root, "Enabled", m_enabled);
	GUIDrawer::DrawBoolean(p_root, "Playing", m_playing);
	GUIDrawer::DrawBoolean(p_root, "Looping", m_looping);
	GUIDrawer::DrawScalar<float>(p_root, "Playback Speed", m_playbackSpeed, 0.01f, -10.0f, 10.0f);
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
	auto& animationChoice = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(m_animationIndex);
	animationChoice.choices.emplace(-1, "<None>");

	for (size_t i = 0; i < m_animationNames.size(); ++i)
	{
		animationChoice.choices.emplace(static_cast<int>(i), m_animationNames[i]);
	}

	animationChoice.ValueChangedEvent += [this](int p_choice)
	{
		if (p_choice < 0)
		{
			m_animationIndex = -1;
			m_currentTimeTicks = 0.0f;
			m_poseEvaluationAccumulator = 0.0f;
			EvaluatePose();
		}
		else
		{
			SetAnimation(static_cast<uint32_t>(p_choice));
		}
	};

	if (!HasCompatibleModel())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::Text>("No skinned model assigned");
	}
	else if (m_animationNames.empty())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::Text>("Model has no animation clips");
	}
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
	const int32_t preservedAnimationIndex = m_animationIndex;
	const std::string requestedAnimationName = m_deserializedAnimationName;

	m_animationNames.clear();
	m_bindPoseTRS.clear();
	m_localPose.clear();
	m_globalPose.clear();
	m_boneMatrices.clear();
	m_boneMatricesTransposed.clear();
	m_trackSamplingCursors.clear();
	m_animationIndex = -1;
	m_currentTimeTicks = preservedTimeTicks;
	m_poseEvaluationAccumulator = 0.0f;
	m_lastSampleTimeTicks = 0.0f;
	m_lastSampledAnimationIndex = -1;

	if (!HasCompatibleModel())
	{
		return;
	}

	const auto& skeleton = m_model->GetSkeleton().value();
	const auto& animations = m_model->GetAnimations();

	m_bindPoseTRS.reserve(skeleton.nodes.size());
	for (const auto& node : skeleton.nodes)
	{
		OvMaths::FVector3 position = OvMaths::FVector3::Zero;
		OvMaths::FQuaternion rotation = OvMaths::FQuaternion::Identity;
		OvMaths::FVector3 scale = OvMaths::FVector3::One;
		DecomposeTRS(node.localBindTransform, position, rotation, scale);

		m_bindPoseTRS.push_back({
			.position = position,
			.rotation = rotation,
			.scale = scale
		});
	}

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
			m_animationIndex = found != m_animationNames.end() ? static_cast<int32_t>(std::distance(m_animationNames.begin(), found)) : 0;
		}
		else if (preservedAnimationIndex >= 0 && static_cast<size_t>(preservedAnimationIndex) < animations.size())
		{
			m_animationIndex = preservedAnimationIndex;
		}
	}

	if (m_animationIndex >= 0 && static_cast<size_t>(m_animationIndex) < animations.size())
	{
		const auto& animation = animations.at(static_cast<size_t>(m_animationIndex));
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

	if (m_bindPoseTRS.size() != skeleton.nodes.size())
	{
		m_bindPoseTRS.clear();
		m_bindPoseTRS.reserve(skeleton.nodes.size());

		for (const auto& node : skeleton.nodes)
		{
			OvMaths::FVector3 position = OvMaths::FVector3::Zero;
			OvMaths::FQuaternion rotation = OvMaths::FQuaternion::Identity;
			OvMaths::FVector3 scale = OvMaths::FVector3::One;
			DecomposeTRS(node.localBindTransform, position, rotation, scale);

			m_bindPoseTRS.push_back({
				.position = position,
				.rotation = rotation,
				.scale = scale
			});
		}
	}

	if (m_localPose.size() != skeleton.nodes.size())
	{
		m_localPose.assign(skeleton.nodes.size(), OvMaths::FMatrix4::Identity);
	}

	if (m_globalPose.size() != skeleton.nodes.size())
	{
		m_globalPose.assign(skeleton.nodes.size(), OvMaths::FMatrix4::Identity);
	}

	if (m_boneMatrices.size() != skeleton.bones.size())
	{
		m_boneMatrices.assign(skeleton.bones.size(), OvMaths::FMatrix4::Identity);
	}

	if (m_boneMatricesTransposed.size() != skeleton.bones.size())
	{
		m_boneMatricesTransposed.assign(skeleton.bones.size(), OvMaths::FMatrix4::Identity);
	}

	for (size_t nodeIndex = 0; nodeIndex < skeleton.nodes.size(); ++nodeIndex)
	{
		m_localPose[nodeIndex] = skeleton.nodes[nodeIndex].localBindTransform;
	}

	if (m_animationIndex >= 0 && static_cast<size_t>(m_animationIndex) < m_model->GetAnimations().size())
	{
		const auto& animation = m_model->GetAnimations().at(static_cast<size_t>(m_animationIndex));
		const float duration = std::max(animation.duration, 0.0f);
		const float sampleTime =
			duration > 0.0f ?
			(m_looping ? WrapTime(m_currentTimeTicks, duration) : std::clamp(m_currentTimeTicks, 0.0f, duration)) :
			0.0f;

		if (m_trackSamplingCursors.size() != animation.tracks.size())
		{
			m_trackSamplingCursors.assign(animation.tracks.size(), TrackSamplingCursor{});
		}

		const bool useSamplingCursorHint =
			m_lastSampledAnimationIndex == m_animationIndex &&
			sampleTime >= m_lastSampleTimeTicks;

		if (!useSamplingCursorHint)
		{
			for (auto& cursor : m_trackSamplingCursors)
			{
				cursor = {};
			}
		}

		for (size_t trackIndex = 0; trackIndex < animation.tracks.size(); ++trackIndex)
		{
			const auto& track = animation.tracks[trackIndex];
			auto& trackCursor = m_trackSamplingCursors[trackIndex];

			if (track.nodeIndex >= m_localPose.size() || track.nodeIndex >= m_bindPoseTRS.size())
			{
				continue;
			}

			const auto& bindTRS = m_bindPoseTRS[track.nodeIndex];
			TRS sampled{
				.position = SampleKeys(
					track.positionKeys,
					sampleTime,
					duration,
					bindTRS.position,
					m_looping,
					[](const auto& p_a, const auto& p_b, float p_alpha) { return OvMaths::FVector3::Lerp(p_a, p_b, p_alpha); },
					&trackCursor.positionKeyIndex,
					useSamplingCursorHint
				),
				.rotation = SampleKeys(
					track.rotationKeys,
					sampleTime,
					duration,
					bindTRS.rotation,
					m_looping,
					[](const auto& p_a, const auto& p_b, float p_alpha) { return OvMaths::FQuaternion::Slerp(p_a, p_b, p_alpha); },
					&trackCursor.rotationKeyIndex,
					useSamplingCursorHint
				),
				.scale = SampleKeys(
					track.scaleKeys,
					sampleTime,
					duration,
					bindTRS.scale,
					m_looping,
					[](const auto& p_a, const auto& p_b, float p_alpha) { return OvMaths::FVector3::Lerp(p_a, p_b, p_alpha); },
					&trackCursor.scaleKeyIndex,
					useSamplingCursorHint
				)
			};

			m_localPose[track.nodeIndex] = ComposeTRS(sampled.position, sampled.rotation, sampled.scale);
		}

		m_lastSampledAnimationIndex = m_animationIndex;
		m_lastSampleTimeTicks = sampleTime;
	}
	else
	{
		m_lastSampledAnimationIndex = -1;
		m_lastSampleTimeTicks = 0.0f;
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
		if (bone.nodeIndex < m_globalPose.size())
		{
			m_boneMatrices[boneIndex] =
				m_globalPose[bone.nodeIndex] *
				bone.offsetMatrix;
		}
		else
		{
			m_boneMatrices[boneIndex] = OvMaths::FMatrix4::Identity;
		}

		m_boneMatricesTransposed[boneIndex] = OvMaths::FMatrix4::Transpose(m_boneMatrices[boneIndex]);
	}

	++m_poseVersion;
}

float OvCore::ECS::Components::CSkinnedMeshRenderer::GetAnimationDurationSeconds() const
{
	if (!HasCompatibleModel() || m_animationIndex < 0)
	{
		return 0.0f;
	}

	const auto& animation = m_model->GetAnimations().at(static_cast<size_t>(m_animationIndex));
	return animation.GetDurationSeconds();
}

void OvCore::ECS::Components::CSkinnedMeshRenderer::UpdatePlayback(float p_deltaTime)
{
	if (!HasCompatibleModel() || m_animationIndex < 0)
	{
		return;
	}

	const auto& animation = m_model->GetAnimations().at(static_cast<size_t>(m_animationIndex));
	if (animation.duration <= 0.0f)
	{
		return;
	}

	if (std::abs(m_playbackSpeed) <= std::numeric_limits<float>::epsilon())
	{
		return;
	}

	const float ticksPerSecond = animation.ticksPerSecond > 0.0f ? animation.ticksPerSecond : kDefaultTicksPerSecond;
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
