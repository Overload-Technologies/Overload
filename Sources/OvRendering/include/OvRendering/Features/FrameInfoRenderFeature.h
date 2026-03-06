/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvRendering/Core/CompositeRenderer.h"
#include "OvRendering/Features/ARenderFeature.h"
#include "OvRendering/Data/FrameInfo.h"

namespace OvRendering::Features
{
	class FrameInfoRenderFeature : public OvRendering::Features::ARenderFeature
	{
	public:
		/**
		* Constructor
		* @param p_renderer
		* @param p_executionPolicy
		*/
		FrameInfoRenderFeature(
			OvRendering::Core::CompositeRenderer& p_renderer,
			OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
		);

		/**
		* Destructor
		*/
		virtual ~FrameInfoRenderFeature();

		/**
		* Return a reference to the last frame info
		* @note Will throw an error if called during the rendering of a frame
		*/
		const OvRendering::Data::FrameInfo& GetFrameInfo() const;

	protected:
		virtual void OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor) override;
		virtual void OnEndFrame() override;
		virtual void OnAfterDraw(const OvRendering::Entities::Drawable& p_drawable) override;

	private:
		bool m_isFrameInfoDataValid;
		OvRendering::Data::FrameInfo m_frameInfo;
		OvTools::Eventing::ListenerID m_postDrawListener;
	};
}
