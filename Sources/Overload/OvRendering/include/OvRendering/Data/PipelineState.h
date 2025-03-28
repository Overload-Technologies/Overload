/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <bitset>

#include "OvRendering/Settings/ERenderingCapability.h"
#include "OvRendering/Settings/EPrimitiveMode.h"
#include "OvRendering/Settings/ERasterizationMode.h"
#include "OvRendering/Settings/EComparaisonAlgorithm.h"
#include "OvRendering/Settings/EOperation.h"
#include "OvRendering/Settings/ECullFace.h"
#include "OvRendering/Settings/ECullingOptions.h"
#include "OvRendering/Settings/EPixelDataFormat.h"
#include "OvRendering/Settings/EPixelDataType.h"

namespace OvRendering::Data
{
	/**
	* Represents the current state of the driver and allow for efficient context switches
	* @note because we target 64-bit architecture, the data bus can expected to be 8 bytes wide
	* so copying 4 bytes will end-up copying 8 bytes. Therefore, we should try to align this struct
	* to take a multiple of 8 bytes.
	*/
	struct PipelineState
	{
		PipelineState();

		union
		{
			struct
			{
				// B0
				uint8_t stencilWriteMask : 8;

				// B1
				uint8_t stencilFuncRef : 8;

				// B2
				uint8_t stencilFuncMask : 8;

				// B3
				Settings::EComparaisonAlgorithm stencilFuncOp : 3;
				Settings::EOperation stencilOpFail : 3;
				Settings::ECullFace cullFace : 2;

				// B4
				Settings::EOperation depthOpFail : 3;
				Settings::EOperation bothOpFail : 3;
				Settings::ERasterizationMode rasterizationMode : 2;

				// B5
				uint8_t lineWidthPow2 : 3;
				Settings::EComparaisonAlgorithm depthFunc : 2;
				bool depthWriting : 1;
				bool blending : 1;
				bool userInterface : 1;

				// B6
				bool culling : 1;
				bool sampleAlphaToCoverage : 1;
				bool polygonOffsetFill : 1;
				bool multisample : 1;
				bool depthTest : 1;
				bool stencilTest : 1;
				bool scissorTest : 1;
				bool dither : 1;

				// B7
				union
				{
					struct
					{
						bool r : 1;
						bool g : 1;
						bool b : 1;
						bool a : 1;
					};

					uint8_t mask : 4;
				} colorWriting;
			};

			std::bitset<64> bits;

			uint8_t bytes[8];
		};
	};
}
