/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <OvCore/ECS/Components/AComponent.h>
#include <OvMaths/FMatrix4.h>

namespace OvCore::ECS { class Actor; }
namespace OvRendering::Resources { class Model; }

namespace OvCore::ECS::Components
{
	/**
	* Component responsible for skeletal animation playback and skinning data generation.
	*/
	class CSkinnedMeshRenderer : public AComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CSkinnedMeshRenderer(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

		/**
		* Called by the model renderer when its model changes
		*/
		void NotifyModelChanged();

		/**
		* Returns true if the component has a valid skinning palette ready for rendering
		*/
		bool HasSkinningData() const;

		/**
		* Start animation playback
		*/
		void Play();

		/**
		* Pause animation playback
		*/
		void Pause();

		/**
		* Stop animation playback and reset time to 0
		*/
		void Stop();

		/**
		* Returns true if playback is active
		*/
		bool IsPlaying() const;

		/**
		* Sets loop mode
		* @param p_value
		*/
		void SetLooping(bool p_value);

		/**
		* Returns true if loop mode is enabled
		*/
		bool IsLooping() const;

		/**
		* Set playback speed
		* @param p_value
		*/
		void SetPlaybackSpeed(float p_value);

		/**
		* Get playback speed
		*/
		float GetPlaybackSpeed() const;

		/**
		* Returns the scale applied to mesh bounds during frustum culling
		* Returned value is always >= 1.0f
		*/
		float GetMeshBoundsScale() const;

		/**
		* Sets the scale applied to mesh bounds during frustum culling
		* Any value below 1.0f will be clamped to 1.0f
		* @param p_scale
		*/
		void SetMeshBoundsScale(float p_scale);

		/**
		* Sets the current playback time in seconds
		* @param p_timeSeconds
		*/
		void SetTime(float p_timeSeconds);

		/**
		* Returns the current playback time in seconds
		*/
		float GetTime() const;

		/**
		* Returns the number of available animations
		*/
		uint32_t GetAnimationCount() const;

		/**
		* Returns the animation name at index (std::nullopt if index is invalid)
		* @param p_index
		*/
		std::optional<std::string> GetAnimationName(uint32_t p_index) const;

		/**
		* Sets the active animation by index. Pass std::nullopt to clear and return to T-pose.
		* @param p_index
		*/
		bool SetAnimation(std::optional<uint32_t> p_index);

		/**
		* Sets the active animation by name
		* @param p_name
		*/
		bool SetAnimation(const std::string& p_name);

		/**
		* Returns the active animation index, or std::nullopt if none is set
		*/
		std::optional<uint32_t> GetActiveAnimationIndex() const;

		/**
		* Returns the active animation name (empty if none)
		*/
		std::optional<std::string> GetActiveAnimationName() const;

		/**
		* Returns the transposed skinning matrix palette ready for GPU upload
		*/
		const std::vector<OvMaths::FMatrix4>& GetBoneMatricesTransposed() const;

		/**
		* Returns an incrementing pose version, updated whenever bone matrices change
		*/
		uint64_t GetPoseVersion() const;

		/**
		* Called each frame by the actor
		* @param p_deltaTime
		*/
		void OnUpdate(float p_deltaTime) override;

		/**
		* Serialize the component
		* @param p_doc
		* @param p_node
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Deserialize the component
		* @param p_doc
		* @param p_node
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;

		/**
		* Draw component inspector widgets
		* @param p_root
		*/
		virtual void OnInspector(OvUI::Internal::WidgetContainer& p_root) override;

	private:
		bool HasCompatibleModel() const;
		void SyncWithModel();
		void RebuildRuntimeData();
		void EvaluatePose();
		float GetAnimationDurationSeconds() const;
		void UpdatePlayback(float p_deltaTime);

	private:
		const OvRendering::Resources::Model* m_model = nullptr;

		bool m_playing = true;
		bool m_looping = true;
		float m_playbackSpeed = 1.0f;
		float m_meshBoundsScale = 1.5f;
		float m_poseEvaluationRate = 60.0f;
		float m_poseEvaluationAccumulator = 0.0f;

		float m_currentTimeTicks = 0.0f;
		std::optional<uint32_t> m_animationIndex = std::nullopt;
		std::string m_deserializedAnimationName;

		uint64_t m_poseVersion = 0;

		std::vector<std::string> m_animationNames;
		std::vector<OvMaths::FMatrix4> m_localPose;
		std::vector<OvMaths::FMatrix4> m_globalPose;
		std::vector<OvMaths::FMatrix4> m_boneMatrices;
		std::vector<OvMaths::FMatrix4> m_boneMatricesTransposed;
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CSkinnedMeshRenderer>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CSkinnedMeshRenderer";
	};
}
