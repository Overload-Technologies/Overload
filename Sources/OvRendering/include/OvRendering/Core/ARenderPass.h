/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Data/FrameDescriptor.h>
#include <OvRendering/Settings/ERenderPassOrder.h>

namespace OvRendering::Core
{
	class CompositeRenderer;
}

namespace OvRendering::Core
{
	/**
	* Represents a rendering step in the graphics pipeline.
	* Subclasses of this class define specific rendering passes.
	*/
	class ARenderPass
	{
	public:
		/**
		* Constructor
		* @param p_renderer
		*/
		ARenderPass(Core::CompositeRenderer& p_renderer);

		/**
		* Destructor
		*/
		virtual ~ARenderPass() = default;

		/**
		* Enable (or disable) the render pass.
		*/
		void SetEnabled(bool p_enabled);

		/**
		* Returns true if the render pass is enabled
		*/
		bool IsEnabled() const;

	protected:
		/**
		* Invoked when BeginFrame is called on the associated renderer
		* @param p_frameDescriptor
		*/
		virtual void OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor);

		/**
		* Invoked when EndFrame is called on the associated renderer
		*/
		virtual void OnEndFrame();

		/**
		* Performs the rendering for the pass using the specified PipelineState.
		* @param p_pso
		*/
		virtual void Draw(OvRendering::Data::PipelineState p_pso) = 0;

	protected:
		Core::CompositeRenderer& m_renderer;
		bool m_enabled = true;

		friend class Core::CompositeRenderer;
	};
}