/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/


#include <OvDebug/Assertion.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Rendering/DebugModelRenderFeature.h>
#include <OvEditor/Rendering/GridRenderPass.h>

#include <OvRendering/Features/DebugShapeRenderFeature.h>
#include <OvRendering/HAL/Profiling.h>

OvEditor::Rendering::GridRenderPass::GridRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Core::ARenderPass(p_renderer)
{
	/* Grid Material */
	m_gridMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Grid"));
	m_gridMaterial.SetBlendable(true);
	m_gridMaterial.SetBackfaceCulling(false);
	m_gridMaterial.SetDepthWriting(false);
	m_gridMaterial.SetDepthTest(true);
}

void OvEditor::Rendering::GridRenderPass::Draw(OvRendering::Data::PipelineState p_pso)
{
	TracyGpuZone("GridRenderPass");

	OVASSERT(m_renderer.HasDescriptor<GridDescriptor>(), "Cannot find GridDescriptor attached to this renderer");
	OVASSERT(m_renderer.HasFeature<OvRendering::Features::DebugShapeRenderFeature>(), "Cannot find DebugShapeRenderFeature attached to this renderer");
	OVASSERT(m_renderer.HasFeature<OvEditor::Rendering::DebugModelRenderFeature>(), "Cannot find DebugModelRenderFeature attached to this renderer");

	auto& gridDescriptor = m_renderer.GetDescriptor<GridDescriptor>();
	auto& debugShapeRenderer = m_renderer.GetFeature<OvRendering::Features::DebugShapeRenderFeature>();

	auto pso = m_renderer.CreatePipelineState();

	constexpr float gridSize = 5000.0f;

	OvMaths::FMatrix4 model =
		OvMaths::FMatrix4::Translation({ gridDescriptor.viewPosition.x, 0.0f, gridDescriptor.viewPosition.z }) *
		OvMaths::FMatrix4::Scaling({ gridSize * 2.0f, 1.f, gridSize * 2.0f });

	m_gridMaterial.SetProperty("u_Color", gridDescriptor.gridColor);

	m_renderer.GetFeature<DebugModelRenderFeature>()
		.DrawModelWithSingleMaterial(pso, *EDITOR_CONTEXT(editorResources)->GetModel("Plane"), m_gridMaterial, model);

	constexpr float kLineWidth = 1.0f;

	debugShapeRenderer.DrawLine(pso, OvMaths::FVector3(-gridSize + gridDescriptor.viewPosition.x, 0.0f, 0.0f), OvMaths::FVector3(gridSize + gridDescriptor.viewPosition.x, 0.0f, 0.0f), OvMaths::FVector3::Right, kLineWidth);
	debugShapeRenderer.DrawLine(pso, OvMaths::FVector3(0.0f, -gridSize + gridDescriptor.viewPosition.y, 0.0f), OvMaths::FVector3(0.0f, gridSize + gridDescriptor.viewPosition.y, 0.0f), OvMaths::FVector3::Up, kLineWidth);
	debugShapeRenderer.DrawLine(pso, OvMaths::FVector3(0.0f, 0.0f, -gridSize + gridDescriptor.viewPosition.z), OvMaths::FVector3(0.0f, 0.0f, gridSize + gridDescriptor.viewPosition.z), OvMaths::FVector3::Forward, kLineWidth);
}
