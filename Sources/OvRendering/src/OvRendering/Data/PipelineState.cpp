/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Data/PipelineState.h"

OvRendering::Data::PipelineState::PipelineState() :
	colorWriting{true, true, true, true},
	depthWriting(true),
	culling(true),
	dither(false),
	polygonOffsetFill(false),
	sampleAlphaToCoverage(false),
	depthTest(true),
	scissorTest(false),
	stencilTest(false),
	multisample(true),
	rasterizationMode(baregl::types::ERasterizationMode::FILL),
	stencilFuncOp(baregl::types::EComparaisonAlgorithm::ALWAYS),
	stencilFuncRef(0x00),
	stencilFuncMask(0xFF),
	stencilWriteMask(0xFF),
	stencilOpFail(baregl::types::EOperation::KEEP),
	depthOpFail(baregl::types::EOperation::KEEP),
	bothOpFail(baregl::types::EOperation::KEEP),
	depthFunc(baregl::types::EComparaisonAlgorithm::LESS),
	cullFace(baregl::types::ECullFace::BACK),
	lineWidthPow2(0x00),
	blending(false),
	blendingSrcFactor(baregl::types::EBlendingFactor::SRC_ALPHA),
	blendingEquation(baregl::types::EBlendingEquation::FUNC_ADD),
	blendingDestFactor(baregl::types::EBlendingFactor::ONE_MINUS_SRC_ALPHA)
{
}
