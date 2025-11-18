/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "AComponent.h"

#include <OvTools/Eventing/Event.h>

#include <OvAudio/Entities/AudioListener.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* Represents an audio listener. The distance between audio sources and audio listener will affect the sound
	* attenuation of spatialized sounds
	*/
	class CAudioListener : public AComponent
	{
	public:
		/**
		* Constructor
		* @param p_owner
		*/
		CAudioListener(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Returns the type name of the component
		*/
		virtual std::string GetTypeName() override;

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
		* Defines how the component should be drawn in the inspector
		* @param p_root
		*/
		virtual void OnInspector(OvUI::Internal::WidgetContainer& p_root) override;

	private:
		virtual void OnEnable() override;
		virtual void OnDisable() override;

	private:
		OvAudio::Entities::AudioListener m_audioListener;
	};

	template<>
	struct ComponentTraits<OvCore::ECS::Components::CAudioListener>
	{
		static constexpr std::string_view Name = "class OvCore::ECS::Components::CAudioListener";
	};
}