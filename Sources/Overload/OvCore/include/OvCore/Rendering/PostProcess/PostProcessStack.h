/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Rendering/PostProcess/AEffect.h>

namespace OvCore::Rendering::PostProcess
{
	/**
	* Set of settings for post process effects
	*/
	class PostProcessStack
	{
	public:
		/**
		* Create a post process stack with default settings for each effect
		*/
		PostProcessStack();

		/**
		* Update settings for the given effect type
		* @param p_settings
		*/
		template <typename Effect, typename Settings>
		void Set(const Settings& p_settings);

		/**
		* Returns the effect settings instance (cast to the given type) associated with a given effect type
		*/
		template <typename Effect, typename Settings>
		Settings& Get();

		/**
		* Returns the effect settings instance (cast to the given type & const) associated with a given effect type
		*/
		template <typename Effect, typename Settings>
		const Settings& Get() const;

		/**
		* Returns the effect settings instance associated with a given effect type
		* @param p_type
		*/
		EffectSettings& Get(const std::type_index& p_type);

		/**
		* Returns the effect settings instance (const) associated with a given effect type
		* @param p_type
		*/
		const EffectSettings& Get(const std::type_index& p_type) const;

	private:
		std::unordered_map<std::type_index, std::unique_ptr<EffectSettings>> m_settings;
	};
}

#include <OvCore/Rendering/PostProcess/PostProcessStack.inl>
