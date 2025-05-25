/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/AComponent.h>
#include <OvRendering/HAL/Framebuffer.h>
#include <OvRendering/HAL/Texture.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::Rendering
{
	class ReflectionRenderPass;
	class ReflectionRenderFeature;
}

namespace OvCore::ECS::Components
{
	/**
	* A simple light that has no attenuation and that has a direction
	*/
	class CReflectionProbe : public AComponent
	{
	public:
		enum class ERefreshMode : int // do not change the type `int`, serialization depends on it
		{
			REALTIME,
			ONCE,
			MANUAL
		};

		/**
		* Constructor
		* @param p_owner
		*/
		CReflectionProbe(ECS::Actor& p_owner);

		/**
		* Returns the name of the component
		*/
		std::string GetName() override;

		/**
		* Sets the refresh mode of the reflection probe
		* @param p_mode
		*/
		void SetRefreshMode(ERefreshMode p_mode);

		/**
		* Returns the refresh mode of the reflection probe
		*/
		ERefreshMode GetRefreshMode() const;

		/**
		* Determines if the reflection probe should capture dynamic objects
		* @param p_capture
		*/
		void SetCaptureDynamicObjects(bool p_capture);

		/**
		* Returns true if the reflection probe should capture dynamic objects
		*/
		bool GetCaptureDynamicObjects() const;

		/**
		* Sets the size of the influence volume of the reflection probe
		* @param p_size
		*/
		void SetInfluenceSize(const OvMaths::FVector3& p_size);

		/**
		* Returns the size of the reflection probe volume
		*/
		const OvMaths::FVector3& GetInfluenceSize() const;

		/**
		* Sets the position offset of the influence volume of the reflection probe, based on the actor position
		* @param p_offset
		*/
		void SetInfluenceOffset(const OvMaths::FVector3& p_offset);

		/**
		* Returns the position offset of the reflection probe volume
		*/
		const OvMaths::FVector3& GetInfluenceOffset() const;

		/**
		* Sets the cubemap resolution
		* @note The resolution must be a power of 2!
		* @param p_resolution
		*/
		void SetCubemapResolution(uint32_t p_resolution);

		/**
		* Returns the cubemap resolution
		*/
		uint32_t GetCubemapResolution() const;

		/**
		* Requests the cubemap to be updated
		*/
		void RequestCapture();

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

	protected:
		virtual void OnEnable() override;

	private:
		bool _IsCaptureRequested() const;
		void _MarkCaptureRequestComplete();
		void _CreateCubemap();
		std::shared_ptr<OvRendering::HAL::Texture> _GetCubemap() const;
		OvRendering::HAL::Framebuffer& _GetFramebuffer() const;

		friend class OvCore::Rendering::ReflectionRenderPass;
		friend class OvCore::Rendering::ReflectionRenderFeature;

	private:
		std::unique_ptr<OvRendering::HAL::Framebuffer> m_framebuffer;
		std::shared_ptr<OvRendering::HAL::Texture> m_cubemap;
		ERefreshMode m_refreshMode = ERefreshMode::ONCE;
		bool m_captureRequested = false;
		bool m_captureDynamicObjects = false;
		uint32_t m_resolution = 512;
		OvMaths::FVector3 m_influenceSize{ 10.0f, 10.0f, 10.0f };
		OvMaths::FVector3 m_influenceOffset{ 0.0f, 0.0f, 0.0f };
	};
}
