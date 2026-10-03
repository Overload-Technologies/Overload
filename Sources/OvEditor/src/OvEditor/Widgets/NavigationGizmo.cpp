/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>

#include <imgui.h>

#include <OvEditor/Core/CameraController.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Widgets/NavigationGizmo.h>
#include <OvMaths/FVector3.h>
#include <OvRendering/Entities/Camera.h>

namespace
{
    struct Bubble
    {
        OvMaths::FVector3 axis;
        ImVec4 color;
        const char* label; // only positive axes are labelled
    };

    struct ProjectedBubble
    {
        const Bubble* bubble = nullptr;
        ImVec2 position;
        float depth = 0.0f; // positive when the bubble points away from the viewer
    };

    constexpr float kAxisLength = 36.0f;
    constexpr float kBubbleRadius = 9.0f;
    constexpr float kMargin = 10.0f;
    constexpr int kCircleSegments = 24;

    float Dot(const OvMaths::FVector3& p_left, const OvMaths::FVector3& p_right)
    {
        return p_left.x * p_right.x + p_left.y * p_right.y + p_left.z * p_right.z;
    }
}

OvEditor::Widgets::NavigationGizmo::NavigationGizmo(
    OvRendering::Entities::Camera& p_camera,
    OvEditor::Core::CameraController& p_cameraController
) :
    m_camera(p_camera),
    m_cameraController(p_cameraController)
{
}

bool OvEditor::Widgets::NavigationGizmo::IsHovered() const
{
    return m_hovered;
}

void OvEditor::Widgets::NavigationGizmo::_Draw_Impl()
{
    static const std::array<Bubble, 6> kBubbles = { {
        { OvMaths::FVector3(1.0f, 0.0f, 0.0f), ImVec4(1.00f, 0.21f, 0.33f, 1.0f), "X" },
        { OvMaths::FVector3(0.0f, 1.0f, 0.0f), ImVec4(0.54f, 0.86f, 0.00f, 1.0f), "Y" },
        { OvMaths::FVector3(0.0f, 0.0f, 1.0f), ImVec4(0.17f, 0.56f, 1.00f, 1.0f), "Z" },
        { OvMaths::FVector3(-1.0f, 0.0f, 0.0f), ImVec4(1.00f, 0.21f, 0.33f, 1.0f), nullptr },
        { OvMaths::FVector3(0.0f, -1.0f, 0.0f), ImVec4(0.54f, 0.86f, 0.00f, 1.0f), nullptr },
        { OvMaths::FVector3(0.0f, 0.0f, -1.0f), ImVec4(0.17f, 0.56f, 1.00f, 1.0f), nullptr },
    } };

    const float scale = EDITOR_CONTEXT(uiManager)->GetScale();
    const float axisLength = kAxisLength * scale;
    const float bubbleRadius = kBubbleRadius * scale;
    const float gizmoRadius = axisLength + bubbleRadius;
    const float margin = kMargin * scale;

    // The gizmo is anchored to the top-right corner of the last drawn item
    const ImVec2 viewMin = ImGui::GetItemRectMin();
    const ImVec2 viewMax = ImGui::GetItemRectMax();
    const float minimumExtent = (gizmoRadius + margin) * 2.0f;

    if (viewMax.x - viewMin.x < minimumExtent || viewMax.y - viewMin.y < minimumExtent)
    {
        m_hovered = false;
        return;
    }

    const ImVec2 center(viewMax.x - margin - gizmoRadius, viewMin.y + margin + gizmoRadius);

    // Capture interaction over the gizmo area
    ImGui::SetCursorScreenPos(ImVec2(center.x - gizmoRadius, center.y - gizmoRadius));
    ImGui::InvisibleButton("##NavigationGizmo", ImVec2(gizmoRadius * 2.0f, gizmoRadius * 2.0f));
    const bool areaHovered = ImGui::IsItemHovered();
    const bool areaActive = ImGui::IsItemActive();
    const bool areaClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    const ImVec2 mouse = ImGui::GetMousePos();
    const float mouseOffsetX = mouse.x - center.x;
    const float mouseOffsetY = mouse.y - center.y;
	m_hovered = areaActive || (areaHovered && (mouseOffsetX * mouseOffsetX + mouseOffsetY * mouseOffsetY <= gizmoRadius * gizmoRadius));

   

    

    // Project axes on the screen
    const OvMaths::FVector3 cameraRight = m_camera.transform->GetWorldRight();
    const OvMaths::FVector3 cameraUp = m_camera.transform->GetWorldUp();
    const OvMaths::FVector3 cameraForward = m_camera.transform->GetWorldForward();

    std::array<ProjectedBubble, 6> projected{};

    for (size_t index = 0; index < kBubbles.size(); ++index)
    {
        const OvMaths::FVector3& axis = kBubbles[index].axis;

        
        const float screenX = -Dot(axis, cameraRight);
        const float screenY = -Dot(axis, cameraUp);

        projected[index].bubble = &kBubbles[index];
        projected[index].position = ImVec2(center.x + screenX * axisLength, center.y + screenY * axisLength);
        projected[index].depth = Dot(axis, cameraForward);
    }

    // closest bubbles are drawn on top
    std::sort(projected.begin(), projected.end(), [](const ProjectedBubble& p_first, const ProjectedBubble& p_second)
    {
        return p_first.depth > p_second.depth;
    });

    
    int hoveredIndex = -1;

    if (m_hovered)
    {
        for (int index = static_cast<int>(projected.size()) - 1; index >= 0; --index)
        {
            const float offsetX = mouse.x - projected[static_cast<size_t>(index)].position.x;
            const float offsetY = mouse.y - projected[static_cast<size_t>(index)].position.y;

            if (offsetX * offsetX + offsetY * offsetY <= bubbleRadius * bubbleRadius)
            {
                hoveredIndex = index;
                break; 
            }
        }
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (m_hovered)
    {
        drawList->AddCircleFilled(center, gizmoRadius, IM_COL32(255, 255, 255, 25), 48);
    }

    
    for (const ProjectedBubble& projectedBubble : projected)
    {
        if (projectedBubble.bubble->label)
        {
            const ImVec4& color = projectedBubble.bubble->color;
            drawList->AddLine(
                center,
                projectedBubble.position,
                ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, 0.6f)),
                2.0f * scale
            );
        }
    }

    for (size_t index = 0; index < projected.size(); ++index)
    {
        const ProjectedBubble& projectedBubble = projected[index];
        const ImVec4& color = projectedBubble.bubble->color;
        const bool isPositive = projectedBubble.bubble->label != nullptr;
        const bool isHovered = static_cast<int>(index) == hoveredIndex;

        
        const float nearness = 0.5f - 0.5f * projectedBubble.depth;
        const float baseAlpha = isPositive ? 0.65f + 0.35f * nearness : 0.25f + 0.2f * nearness;
        const float alpha = isHovered ? 1.0f : baseAlpha;

        drawList->AddCircleFilled(
            projectedBubble.position,
            bubbleRadius,
            ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, alpha)),
            kCircleSegments
        );

        if (!isPositive)
        {
            drawList->AddCircle(
                projectedBubble.position,
                bubbleRadius,
                ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, 0.8f)),
                kCircleSegments,
                1.5f * scale
            );
        }

        if (isHovered)
        {
            drawList->AddCircle(projectedBubble.position, bubbleRadius, IM_COL32(255, 255, 255, 255), kCircleSegments, 2.0f * scale);
        }

        if (isPositive)
        {
            const ImVec2 textSize = ImGui::CalcTextSize(projectedBubble.bubble->label);
            drawList->AddText(
                ImVec2(projectedBubble.position.x - textSize.x * 0.5f, projectedBubble.position.y - textSize.y * 0.5f),
                IM_COL32(25, 25, 25, 255),
                projectedBubble.bubble->label
            );
        }
    }

    
    if (areaClicked)
    {
        m_dragging = false;
    }

    if (areaActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2.0f))
    {
        if (!m_dragging)
        {
            m_dragging = true;
            m_cameraController.BeginOrbit();
        }

        const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        m_cameraController.Orbit(mouseDelta.x, mouseDelta.y);
    }

    // Snap to an axis only on a clean click(not at the end of a drag)
    if (ImGui::IsItemDeactivated() && !m_dragging && hoveredIndex >= 0)
    {
        m_cameraController.MoveToAxisView(projected[static_cast<size_t>(hoveredIndex)].bubble->axis);
    }

   	if (areaClicked)
	{
		m_dragging = false;
	}
   
	if (areaActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2.0f))
	{
		if (!m_dragging)
		{
			m_dragging = true;
			m_cameraController.BeginOrbit();
		}
   
		const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
		m_cameraController.Orbit(mouseDelta.x, mouseDelta.y);
	}
   
	
	if (ImGui::IsItemDeactivated() && !m_dragging && hoveredIndex >= 0)
	{
		m_cameraController.MoveToAxisView(projected[static_cast<size_t>(hoveredIndex)].bubble->axis);
	}
}