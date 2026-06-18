/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <array>
#include <memory>

#include <OvMaths/FVector4.h>
#include <OvTools/Utils/OptRef.h>

#include <OvRendering/Data/PipelineState.h>
#include <OvRendering/Resources/IMesh.h>
#include <OvRendering/Settings/DriverSettings.h>
#include <OvRendering/Settings/ECullingOptions.h>

#include <baregl/Context.h>
#include <baregl/types/ERenderingCapability.h>
#include <baregl/types/EPrimitiveMode.h>
#include <baregl/types/ERasterizationMode.h>
#include <baregl/types/EComparaisonAlgorithm.h>
#include <baregl/types/EOperation.h>
#include <baregl/types/ECullFace.h>
#include <baregl/types/EPixelDataFormat.h>
#include <baregl/types/EPixelDataType.h>

namespace OvRendering::Context
{
	/**
	* Handles the lifecycle of the underlying graphics context
	*/
	class Driver final
	{
	public:
		/**
		* Creates the driver
		* @param p_driverSettings
		*/
		Driver(const Settings::DriverSettings& p_driverSettings);

		/**
		* Destroy the driver
		*/
		~Driver();

		/**
		* Notifies the driver that the frame is finished
		*/
		void OnFrameCompleted();

		/**
		* Set the viewport
		* @param p_x
		* @param p_y
		* @param p_width
		* @param p_height
		*/
		void SetViewport(
			uint32_t p_x,
			uint32_t p_y,
			uint32_t p_width,
			uint32_t p_height
		);

		/**
		* Clear the screen using the previously defined clear color (With Renderer::SetClearColor()) or by
		* using the OpenGL default one.
		* @param p_colorBuffer
		* @param p_depthBuffer
		* @param p_stencilBuffer
		* @param p_color
		*/
		void Clear(
			bool p_colorBuffer,
			bool p_depthBuffer,
			bool p_stencilBuffer,
			const OvMaths::FVector4& p_color = OvMaths::FVector4::Zero 
		);

		/**
		* Draw a mesh
		* @param p_pso
		* @param p_mesh
		* @param p_primitiveMode
		* @param p_instances
		*/
		void Draw(
			OvRendering::Data::PipelineState p_pso,
			const Resources::IMesh& p_mesh,
			baregl::types::EPrimitiveMode p_primitiveMode = baregl::types::EPrimitiveMode::TRIANGLES,
			uint32_t p_instances = 1
		);

		/**
		* Create a pipeline state from the default state
		*/
		Data::PipelineState CreatePipelineState() const;

		/**
		* Returns the vendor
		*/
		std::string_view GetVendor() const;

		/**
		* Returns details about the current rendering hardware
		*/
		std::string_view GetHardware() const;

		/**
		* Returns the current graphics API version
		*/
		std::string_view GetVersion() const;

		/**
		* Returns the current shading language version
		*/
		std::string_view GetShadingLanguageVersion() const;

	private:
		void SetPipelineState(Data::PipelineState p_state);
		void ResetPipelineState();

	private:
		std::unique_ptr<baregl::Context> m_gfxContext;
		std::string m_vendor;
		std::string m_hardware;
		std::string m_version;
		std::string m_shadingLanguageVersion;
		Data::PipelineState m_defaultPipelineState;
		Data::PipelineState m_pipelineState;
	};
}
