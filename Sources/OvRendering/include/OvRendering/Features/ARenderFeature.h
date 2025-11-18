/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <set>

#include <OvRendering/Data/FrameDescriptor.h>
#include <OvRendering/Entities/Drawable.h>
#include <OvRendering/Types/RenderPassType.h>

namespace OvRendering::Core
{
	class CompositeRenderer;
}

namespace OvRendering::Features
{
	/**
	* Defines the execution policy of a render feature.
	*/
	enum class EFeatureExecutionPolicy
	{
		DEFAULT, // Execute if the pass is not blacklisted
		WHITELIST_ONLY, // Only execute if the pass is whitelisted
		ALWAYS, // Ignore blacklist and whitelist
		NEVER, // Never execute, regardless of the pass or the feature state (useful for "function only" features)
		FRAME_EVENTS_ONLY // Only execute the frame events (OnBeginFrame, OnEndFrame) without drawing anything
	};

	/**
	* Extension of the composite renderer that provides new rendering capabilities
	*/
	class ARenderFeature
	{
	public:
		/**
		* Destructor
		*/
		virtual ~ARenderFeature() = default;

		/**
		* Returns true if the render feature has a policy that allows it to be executed (not NEVER).
		*/
		bool IsEnabled() const;

		/**
		* Returns true if this render feature is enabled for the given render pass.
		* @param p_type
		*/
		bool IsEnabledFor(std::type_index p_type) const;

		/**
		* Returns true if this render feature is enabled for the given render pass.
		*/
		template<Types::RenderPassType T>
		bool IsEnabledFor() const { return IsEnabledFor(typeid(T)); }

		/**
		* Add a given render pass to the blacklist.
		* Returns a reference to this render feature for chaining.
		* @param p_type
		*/
		ARenderFeature& Except(std::type_index p_type);

		/**
		* Add a given render pass to the blacklist.
		* Returns a reference to this render feature for chaining.
		*/
		template<Types::RenderPassType T>
		ARenderFeature& Except() { return Except(typeid(T)); }

		/**
		* Add a given render pass to the whitelist.
		* Returns a reference to this render feature for chaining.
		* @param p_type
		*/
		ARenderFeature& Include(std::type_index p_type);

		/**
		* Add a given render pass to the whitelist.
		* Returns a reference to this render feature for chaining.
		*/
		template<Types::RenderPassType T>
		ARenderFeature& Include() { return Include(typeid(T)); }

		/**
		* Defines the execution policy of this render feature.
		* @param p_policy
		*/
		void SetExecutionPolicy(EFeatureExecutionPolicy p_policy);

		/**
		* Gets the execution policy of this render feature.
		*/
		EFeatureExecutionPolicy GetExecutionPolicy() const;

	protected:
		/**
		* Constructor (protected, to be called by the composite renderer)
		* @param p_renderer
		*/
		ARenderFeature(Core::CompositeRenderer& p_renderer, EFeatureExecutionPolicy p_executionPolicy);

		/**
		* Invoked when the BeginFrame is called on the associated base renderer
		* @note Won't be called if the feature execution policy is set to NEVER.
		* @param p_frameDescriptor
		*/
		virtual void OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor);

		/**
		* Invoked when the EndFrame is called on the associated base renderer
		* @note Won't be called if the feature execution policy is set to NEVER.
		*/
		virtual void OnEndFrame();

		/**
		* Invoked before drawing a drawable entity
		* @note Won't be called if the feature execution policy is set to NEVER.
		* @param p_drawable
		*/
		virtual void OnBeforeDraw(Data::PipelineState& p_pso, const Entities::Drawable& p_drawable);

		/**
		* Invoked after drawing a drawable entity
		* @note Won't be called if the feature execution policy is set to NEVER.
		* @param p_drawable
		*/
		virtual void OnAfterDraw(const Entities::Drawable& p_drawable);

	protected:
		Core::CompositeRenderer& m_renderer;

	private:
		std::unordered_set<std::type_index> m_blacklist;
		std::unordered_set<std::type_index> m_whitelist;
		EFeatureExecutionPolicy m_executionPolicy = EFeatureExecutionPolicy::DEFAULT;

		friend class Core::CompositeRenderer;
	};
}