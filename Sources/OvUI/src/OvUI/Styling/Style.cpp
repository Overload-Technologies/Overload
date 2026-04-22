/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Internal/Converter.h>
#include <OvUI/Styling/Style.h>

namespace
{
	ImVec4 ToImVec4(const OvUI::Types::Color& p_color)
	{
		return { p_color.r, p_color.g, p_color.b, p_color.a };
	}
}

OvUI::Styling::Style& OvUI::Styling::Style::Get()
{
	static Style instance;
	return instance;
}

OvUI::Styling::Style::Style()
{
	ApplyDefaultDarkStyle();
}

void OvUI::Styling::Style::PopulateFromImGuiStyle(const ImGuiStyle& p_style)
{
	using namespace OvUI::Internal;

	Text                        = Converter::ToColor(p_style.Colors[ImGuiCol_Text]);
	TextDisabled                = Converter::ToColor(p_style.Colors[ImGuiCol_TextDisabled]);
	WindowBg                    = Converter::ToColor(p_style.Colors[ImGuiCol_WindowBg]);
	ChildBg                     = Converter::ToColor(p_style.Colors[ImGuiCol_ChildBg]);
	PopupBg                     = Converter::ToColor(p_style.Colors[ImGuiCol_PopupBg]);
	Border                      = Converter::ToColor(p_style.Colors[ImGuiCol_Border]);
	BorderShadow                = Converter::ToColor(p_style.Colors[ImGuiCol_BorderShadow]);
	FrameBg                     = Converter::ToColor(p_style.Colors[ImGuiCol_FrameBg]);
	FrameBgHovered              = Converter::ToColor(p_style.Colors[ImGuiCol_FrameBgHovered]);
	FrameBgActive               = Converter::ToColor(p_style.Colors[ImGuiCol_FrameBgActive]);
	TitleBg                     = Converter::ToColor(p_style.Colors[ImGuiCol_TitleBg]);
	TitleBgActive               = Converter::ToColor(p_style.Colors[ImGuiCol_TitleBgActive]);
	TitleBgCollapsed            = Converter::ToColor(p_style.Colors[ImGuiCol_TitleBgCollapsed]);
	MenuBarBg                   = Converter::ToColor(p_style.Colors[ImGuiCol_MenuBarBg]);
	ScrollbarBg                 = Converter::ToColor(p_style.Colors[ImGuiCol_ScrollbarBg]);
	ScrollbarGrab               = Converter::ToColor(p_style.Colors[ImGuiCol_ScrollbarGrab]);
	ScrollbarGrabHovered        = Converter::ToColor(p_style.Colors[ImGuiCol_ScrollbarGrabHovered]);
	ScrollbarGrabActive         = Converter::ToColor(p_style.Colors[ImGuiCol_ScrollbarGrabActive]);
	CheckMark                   = Converter::ToColor(p_style.Colors[ImGuiCol_CheckMark]);
	SliderGrab                  = Converter::ToColor(p_style.Colors[ImGuiCol_SliderGrab]);
	SliderGrabActive            = Converter::ToColor(p_style.Colors[ImGuiCol_SliderGrabActive]);
	Button                      = Converter::ToColor(p_style.Colors[ImGuiCol_Button]);
	ButtonHovered               = Converter::ToColor(p_style.Colors[ImGuiCol_ButtonHovered]);
	ButtonActive                = Converter::ToColor(p_style.Colors[ImGuiCol_ButtonActive]);
	Header                      = Converter::ToColor(p_style.Colors[ImGuiCol_Header]);
	HeaderHovered               = Converter::ToColor(p_style.Colors[ImGuiCol_HeaderHovered]);
	HeaderActive                = Converter::ToColor(p_style.Colors[ImGuiCol_HeaderActive]);
	Separator                   = Converter::ToColor(p_style.Colors[ImGuiCol_Separator]);
	SeparatorHovered            = Converter::ToColor(p_style.Colors[ImGuiCol_SeparatorHovered]);
	SeparatorActive             = Converter::ToColor(p_style.Colors[ImGuiCol_SeparatorActive]);
	ResizeGrip                  = Converter::ToColor(p_style.Colors[ImGuiCol_ResizeGrip]);
	ResizeGripHovered           = Converter::ToColor(p_style.Colors[ImGuiCol_ResizeGripHovered]);
	ResizeGripActive            = Converter::ToColor(p_style.Colors[ImGuiCol_ResizeGripActive]);
	InputTextCursor             = Converter::ToColor(p_style.Colors[ImGuiCol_InputTextCursor]);
	TabHovered                  = Converter::ToColor(p_style.Colors[ImGuiCol_TabHovered]);
	Tab                         = Converter::ToColor(p_style.Colors[ImGuiCol_Tab]);
	TabSelected                 = Converter::ToColor(p_style.Colors[ImGuiCol_TabSelected]);
	TabSelectedOverline         = Converter::ToColor(p_style.Colors[ImGuiCol_TabSelectedOverline]);
	TabDimmed                   = Converter::ToColor(p_style.Colors[ImGuiCol_TabDimmed]);
	TabDimmedSelected           = Converter::ToColor(p_style.Colors[ImGuiCol_TabDimmedSelected]);
	TabDimmedSelectedOverline   = Converter::ToColor(p_style.Colors[ImGuiCol_TabDimmedSelectedOverline]);
	DockingPreview              = Converter::ToColor(p_style.Colors[ImGuiCol_DockingPreview]);
	DockingEmptyBg              = Converter::ToColor(p_style.Colors[ImGuiCol_DockingEmptyBg]);
	PlotLines                   = Converter::ToColor(p_style.Colors[ImGuiCol_PlotLines]);
	PlotLinesHovered            = Converter::ToColor(p_style.Colors[ImGuiCol_PlotLinesHovered]);
	PlotHistogram               = Converter::ToColor(p_style.Colors[ImGuiCol_PlotHistogram]);
	PlotHistogramHovered        = Converter::ToColor(p_style.Colors[ImGuiCol_PlotHistogramHovered]);
	TableHeaderBg               = Converter::ToColor(p_style.Colors[ImGuiCol_TableHeaderBg]);
	TableBorderStrong           = Converter::ToColor(p_style.Colors[ImGuiCol_TableBorderStrong]);
	TableBorderLight            = Converter::ToColor(p_style.Colors[ImGuiCol_TableBorderLight]);
	TableRowBg                  = Converter::ToColor(p_style.Colors[ImGuiCol_TableRowBg]);
	TableRowBgAlt               = Converter::ToColor(p_style.Colors[ImGuiCol_TableRowBgAlt]);
	TextLink                    = Converter::ToColor(p_style.Colors[ImGuiCol_TextLink]);
	TextSelectedBg              = Converter::ToColor(p_style.Colors[ImGuiCol_TextSelectedBg]);
	TreeLines                   = Converter::ToColor(p_style.Colors[ImGuiCol_TreeLines]);
	DragDropTarget              = Converter::ToColor(p_style.Colors[ImGuiCol_DragDropTarget]);
	DragDropTargetBg            = Converter::ToColor(p_style.Colors[ImGuiCol_DragDropTargetBg]);
	UnsavedMarker               = Converter::ToColor(p_style.Colors[ImGuiCol_UnsavedMarker]);
	NavCursor                   = Converter::ToColor(p_style.Colors[ImGuiCol_NavCursor]);
	NavWindowingHighlight       = Converter::ToColor(p_style.Colors[ImGuiCol_NavWindowingHighlight]);
	NavWindowingDimBg           = Converter::ToColor(p_style.Colors[ImGuiCol_NavWindowingDimBg]);
	ModalWindowDimBg            = Converter::ToColor(p_style.Colors[ImGuiCol_ModalWindowDimBg]);

	WindowPadding    = Internal::Converter::ToFVector2(p_style.WindowPadding);
	WindowRounding   = p_style.WindowRounding;
	WindowBorderSize = p_style.WindowBorderSize;
	FramePadding     = Internal::Converter::ToFVector2(p_style.FramePadding);
	FrameRounding    = p_style.FrameRounding;
	FrameBorderSize  = p_style.FrameBorderSize;
	ItemSpacing      = Internal::Converter::ToFVector2(p_style.ItemSpacing);
	ItemInnerSpacing = Internal::Converter::ToFVector2(p_style.ItemInnerSpacing);
	IndentSpacing    = p_style.IndentSpacing;
	ScrollbarSize    = p_style.ScrollbarSize;
	ScrollbarRounding= p_style.ScrollbarRounding;
	GrabMinSize      = p_style.GrabMinSize;
	GrabRounding     = p_style.GrabRounding;
	TabRounding      = p_style.TabRounding;
	ChildRounding    = p_style.ChildRounding;
	PopupRounding    = p_style.PopupRounding;
	PopupBorderSize  = p_style.PopupBorderSize;
}

void OvUI::Styling::Style::SetSemanticDefaults()
{
	// --- Semantic buttons (dark themes) ---
	PrimaryButton         = { 0.26f, 0.59f, 0.98f, 0.80f };
	PrimaryButtonHovered  = { 0.26f, 0.59f, 0.98f, 1.00f };
	PrimaryButtonActive   = { 0.06f, 0.53f, 0.98f, 1.00f };

	SuccessButton         = { 0.15f, 0.49f, 0.15f, 1.00f };
	SuccessButtonHovered  = { 0.20f, 0.64f, 0.20f, 1.00f };
	SuccessButtonActive   = { 0.11f, 0.37f, 0.11f, 1.00f };

	DangerButton          = { 0.50f, 0.08f, 0.08f, 1.00f };
	DangerButtonHovered   = { 0.65f, 0.11f, 0.11f, 1.00f };
	DangerButtonActive    = { 0.38f, 0.06f, 0.06f, 1.00f };

	WarningButton         = { 0.70f, 0.50f, 0.00f, 1.00f };
	WarningButtonHovered  = { 0.80f, 0.60f, 0.00f, 1.00f };
	WarningButtonActive   = { 0.55f, 0.38f, 0.00f, 1.00f };

	// --- Log colors (dark themes) ---
	LogDefault            = { 0.90f, 0.90f, 0.90f, 1.00f };
	LogInfo               = { 0.30f, 0.85f, 1.00f, 1.00f };
	LogWarning            = { 1.00f, 0.85f, 0.20f, 1.00f };
	LogError              = { 1.00f, 0.35f, 0.35f, 1.00f };

	// --- Inspector / script-status (dark themes) ---
	InspectorTitle        = { 0.85f, 0.65f, 0.00f, 1.00f };
	StatusDefault         = { 0.75f, 0.75f, 0.75f, 1.00f };
	StatusReady           = { 0.20f, 0.80f, 0.20f, 1.00f };
	StatusError           = { 1.00f, 0.30f, 0.30f, 1.00f };
}

void OvUI::Styling::Style::SetSemanticLightTheme()
{
	// --- Semantic buttons (light theme — brighter hues, readable with dark text) ---
	PrimaryButton         = { 0.29f, 0.62f, 0.98f, 1.00f };
	PrimaryButtonHovered  = { 0.40f, 0.70f, 1.00f, 1.00f };
	PrimaryButtonActive   = { 0.20f, 0.50f, 0.90f, 1.00f };

	SuccessButton         = { 0.22f, 0.75f, 0.22f, 1.00f };
	SuccessButtonHovered  = { 0.32f, 0.85f, 0.32f, 1.00f };
	SuccessButtonActive   = { 0.15f, 0.62f, 0.15f, 1.00f };

	DangerButton          = { 0.85f, 0.22f, 0.22f, 1.00f };
	DangerButtonHovered   = { 0.92f, 0.35f, 0.35f, 1.00f };
	DangerButtonActive    = { 0.72f, 0.15f, 0.15f, 1.00f };

	WarningButton         = { 0.90f, 0.65f, 0.08f, 1.00f };
	WarningButtonHovered  = { 1.00f, 0.75f, 0.18f, 1.00f };
	WarningButtonActive   = { 0.78f, 0.54f, 0.04f, 1.00f };

	// --- Log colors (light theme — darker tones readable on light bg) ---
	LogDefault            = { 0.15f, 0.15f, 0.15f, 1.00f };
	LogInfo               = { 0.00f, 0.40f, 0.75f, 1.00f };
	LogWarning            = { 0.70f, 0.45f, 0.00f, 1.00f };
	LogError              = { 0.75f, 0.05f, 0.05f, 1.00f };

	// --- Inspector / script-status (light theme) ---
	InspectorTitle        = { 0.55f, 0.35f, 0.00f, 1.00f };
	StatusDefault         = { 0.25f, 0.25f, 0.25f, 1.00f };
	StatusReady           = { 0.05f, 0.55f, 0.05f, 1.00f };
	StatusError           = { 0.72f, 0.05f, 0.05f, 1.00f };
}

void OvUI::Styling::Style::ApplyImClassicStyle()
{
	ImGuiStyle tmp;
	ImGui::StyleColorsClassic(&tmp);
	PopulateFromImGuiStyle(tmp);
	SetSemanticDefaults();
}

void OvUI::Styling::Style::ApplyImDarkStyle()
{
	ImGuiStyle tmp;
	ImGui::StyleColorsDark(&tmp);
	PopulateFromImGuiStyle(tmp);
	SetSemanticDefaults();
}

void OvUI::Styling::Style::ApplyImLightStyle()
{
	ImGuiStyle tmp;
	ImGui::StyleColorsLight(&tmp);
	PopulateFromImGuiStyle(tmp);
	SetSemanticDefaults();
	SetSemanticLightTheme(); // override with light-theme-appropriate values
}

void OvUI::Styling::Style::ApplyDuneDarkStyle()
{
	// Start from the ImGui dark base, then override with Dune Dark values.
	ApplyImDarkStyle();

	Text                 = { 0.80f, 0.80f, 0.83f, 1.00f };
	TextDisabled         = { 0.24f, 0.23f, 0.29f, 1.00f };
	WindowBg             = { 0.06f, 0.05f, 0.07f, 1.00f };
	ChildBg              = { 0.07f, 0.07f, 0.09f, 1.00f };
	PopupBg              = { 0.07f, 0.07f, 0.09f, 1.00f };
	Border               = { 0.20f, 0.20f, 0.20f, 0.88f };
	BorderShadow         = { 0.92f, 0.91f, 0.88f, 0.00f };
	FrameBg              = { 0.10f, 0.09f, 0.12f, 1.00f };
	FrameBgHovered       = { 0.24f, 0.23f, 0.29f, 1.00f };
	FrameBgActive        = { 0.56f, 0.56f, 0.58f, 1.00f };
	TitleBg              = { 0.10f, 0.09f, 0.12f, 1.00f };
	TitleBgCollapsed     = { 0.30f, 0.30f, 0.30f, 0.75f };
	TitleBgActive        = { 0.07f, 0.07f, 0.09f, 1.00f };
	MenuBarBg            = { 0.10f, 0.09f, 0.12f, 1.00f };
	ScrollbarBg          = { 0.10f, 0.09f, 0.12f, 1.00f };
	ScrollbarGrab        = { 0.80f, 0.80f, 0.83f, 0.31f };
	ScrollbarGrabHovered = { 0.56f, 0.56f, 0.58f, 1.00f };
	ScrollbarGrabActive  = { 0.06f, 0.05f, 0.07f, 1.00f };
	CheckMark            = { 0.80f, 0.80f, 0.83f, 0.31f };
	SliderGrab           = { 0.80f, 0.80f, 0.83f, 0.31f };
	SliderGrabActive     = { 0.06f, 0.05f, 0.07f, 1.00f };
	Button               = { 0.10f, 0.09f, 0.12f, 1.00f };
	ButtonHovered        = { 0.24f, 0.23f, 0.29f, 1.00f };
	ButtonActive         = { 0.56f, 0.56f, 0.58f, 1.00f };
	Header               = { 0.10f, 0.09f, 0.12f, 1.00f };
	HeaderHovered        = { 0.56f, 0.56f, 0.58f, 1.00f };
	HeaderActive         = { 0.06f, 0.05f, 0.07f, 1.00f };
	Separator            = { 0.56f, 0.56f, 0.58f, 1.00f };
	SeparatorHovered     = { 0.24f, 0.23f, 0.29f, 1.00f };
	SeparatorActive      = { 0.56f, 0.56f, 0.58f, 1.00f };
	ResizeGrip           = { 0.00f, 0.00f, 0.00f, 0.00f };
	ResizeGripHovered    = { 0.56f, 0.56f, 0.58f, 1.00f };
	ResizeGripActive     = { 0.06f, 0.05f, 0.07f, 1.00f };
	Tab                  = TabDimmed; // mirrors original TStyle behavior
	PlotLines            = { 0.40f, 0.39f, 0.38f, 0.63f };
	PlotLinesHovered     = { 0.25f, 1.00f, 0.00f, 1.00f };
	PlotHistogram        = { 0.40f, 0.39f, 0.38f, 0.63f };
	PlotHistogramHovered = { 0.25f, 1.00f, 0.00f, 1.00f };
	TextSelectedBg       = { 0.25f, 1.00f, 0.00f, 0.43f };
	ModalWindowDimBg     = { 1.00f, 0.98f, 0.95f, 0.73f };

	WindowPadding    = { 15.0f, 15.0f };
	WindowRounding   = 5.0f;
	FramePadding     = { 5.0f, 5.0f };
	FrameRounding    = 4.0f;
	ItemSpacing      = { 12.0f, 8.0f };
	ItemInnerSpacing = { 8.0f, 6.0f };
	IndentSpacing    = 25.0f;
	ScrollbarSize    = 15.0f;
	ScrollbarRounding= 9.0f;
	GrabMinSize      = 5.0f;
	GrabRounding     = 3.0f;
}

void OvUI::Styling::Style::ApplyDefaultDarkStyle()
{
	Text                        = { 0.96f, 0.96f, 0.99f, 1.00f };
	TextDisabled                = { 0.50f, 0.50f, 0.50f, 1.00f };
	WindowBg                    = { 0.09f, 0.09f, 0.10f, 1.00f };
	ChildBg                     = { 0.09f, 0.09f, 0.10f, 1.00f };
	PopupBg                     = { 0.06f, 0.06f, 0.07f, 1.00f };
	Border                      = { 0.12f, 0.12f, 0.14f, 1.00f };
	BorderShadow                = { 0.00f, 0.00f, 0.00f, 0.00f };
	FrameBg                     = { 0.12f, 0.12f, 0.13f, 1.00f };
	FrameBgHovered              = { 0.20f, 0.20f, 0.22f, 1.00f };
	FrameBgActive               = { 0.27f, 0.27f, 0.29f, 1.00f };
	TitleBg                     = { 0.07f, 0.07f, 0.07f, 1.00f };
	TitleBgActive               = { 0.07f, 0.07f, 0.07f, 1.00f };
	TitleBgCollapsed            = { 0.07f, 0.07f, 0.07f, 1.00f };
	MenuBarBg                   = { 0.07f, 0.07f, 0.07f, 1.00f };
	ScrollbarBg                 = { 0.07f, 0.07f, 0.07f, 1.00f };
	ScrollbarGrab               = { 0.31f, 0.31f, 0.32f, 1.00f };
	ScrollbarGrabHovered        = { 0.41f, 0.41f, 0.42f, 1.00f };
	ScrollbarGrabActive         = { 0.51f, 0.51f, 0.53f, 1.00f };
	CheckMark                   = { 0.44f, 0.44f, 0.47f, 1.00f };
	SliderGrab                  = { 0.44f, 0.44f, 0.47f, 1.00f };
	SliderGrabActive            = { 0.59f, 0.59f, 0.61f, 1.00f };
	Button                      = { 0.20f, 0.20f, 0.22f, 1.00f };
	ButtonHovered               = { 0.44f, 0.44f, 0.47f, 1.00f };
	ButtonActive                = { 0.59f, 0.59f, 0.61f, 1.00f };
	Header                      = { 0.20f, 0.20f, 0.22f, 1.00f };
	HeaderHovered               = { 0.44f, 0.44f, 0.47f, 1.00f };
	HeaderActive                = { 0.59f, 0.59f, 0.61f, 1.00f };
	Separator                   = { 1.00f, 1.00f, 1.00f, 0.20f };
	SeparatorHovered            = { 0.44f, 0.44f, 0.47f, 0.39f };
	SeparatorActive             = { 0.44f, 0.44f, 0.47f, 0.59f };
	ResizeGrip                  = { 0.26f, 0.59f, 0.98f, 0.00f };
	ResizeGripHovered           = { 0.26f, 0.59f, 0.98f, 0.00f };
	ResizeGripActive            = { 0.26f, 0.59f, 0.98f, 0.00f };
	InputTextCursor             = { 1.00f, 1.00f, 1.00f, 1.00f };
	Tab                         = { 0.20f, 0.20f, 0.22f, 1.00f };
	TabHovered                  = { 0.44f, 0.44f, 0.47f, 1.00f };
	TabSelected                 = { 0.44f, 0.44f, 0.47f, 1.00f };
	TabSelectedOverline         = { 0.44f, 0.44f, 0.47f, 1.00f };
	TabDimmed                   = { 0.20f, 0.20f, 0.22f, 0.39f };
	TabDimmedSelected           = { 0.44f, 0.44f, 0.47f, 0.39f };
	TabDimmedSelectedOverline   = { 0.44f, 0.44f, 0.47f, 0.39f };
	DockingPreview              = { 0.91f, 0.62f, 0.00f, 0.78f };
	DockingEmptyBg              = { 0.20f, 0.20f, 0.20f, 1.00f };
	PlotLines                   = { 0.96f, 0.96f, 0.99f, 1.00f };
	PlotLinesHovered            = { 0.12f, 1.00f, 0.12f, 1.00f };
	PlotHistogram               = { 0.96f, 0.96f, 0.99f, 1.00f };
	PlotHistogramHovered        = { 0.12f, 1.00f, 0.12f, 1.00f };
	TableHeaderBg               = { 0.12f, 0.12f, 0.14f, 1.00f };
	TableBorderStrong           = { 0.20f, 0.20f, 0.22f, 1.00f };
	TableBorderLight            = { 0.15f, 0.15f, 0.17f, 1.00f };
	TableRowBg                  = { 0.00f, 0.00f, 0.00f, 0.00f };
	TableRowBgAlt               = { 1.00f, 1.00f, 1.00f, 0.04f };
	TextLink                    = { 0.26f, 0.59f, 0.98f, 1.00f };
	TextSelectedBg              = { 0.26f, 0.59f, 0.98f, 0.35f };
	TreeLines                   = { 0.50f, 0.50f, 0.50f, 1.00f };
	DragDropTarget              = { 0.91f, 0.62f, 0.00f, 1.00f };
	DragDropTargetBg            = { 0.26f, 0.59f, 0.98f, 0.20f };
	UnsavedMarker               = { 0.26f, 0.99f, 0.26f, 1.00f };
	NavCursor                   = { 0.26f, 0.59f, 0.98f, 1.00f };
	NavWindowingHighlight       = { 1.00f, 1.00f, 1.00f, 0.70f };
	NavWindowingDimBg           = { 0.80f, 0.80f, 0.80f, 0.20f };
	ModalWindowDimBg            = { 0.80f, 0.80f, 0.80f, 0.35f };

	WindowPadding    = { 15.0f, 15.0f };
	WindowRounding   = 0.0f;
	WindowBorderSize = 1.0f;
	FramePadding     = { 5.0f, 5.0f };
	FrameRounding    = 0.0f;
	FrameBorderSize  = 0.0f;
	ItemSpacing      = { 12.0f, 8.0f };
	ItemInnerSpacing = { 8.0f, 6.0f };
	IndentSpacing    = 25.0f;
	ScrollbarSize    = 15.0f;
	ScrollbarRounding= 0.0f;
	GrabMinSize      = 5.0f;
	GrabRounding     = 0.0f;
	TabRounding      = 0.0f;
	ChildRounding    = 0.0f;
	PopupRounding    = 0.0f;
	PopupBorderSize  = 1.0f;

	SetSemanticDefaults();
}

void OvUI::Styling::Style::ApplyEvenDarkerStyle()
{
	Text                        = { 0.96f, 0.96f, 0.99f, 1.00f };
	TextDisabled                = { 0.50f, 0.50f, 0.50f, 1.00f };
	WindowBg                    = { 0.03f, 0.04f, 0.05f, 1.00f };
	ChildBg                     = { 0.03f, 0.04f, 0.05f, 1.00f };
	PopupBg                     = { 0.03f, 0.04f, 0.05f, 1.00f };
	Border                      = { 0.06f, 0.07f, 0.08f, 1.00f };
	BorderShadow                = { 0.00f, 0.00f, 0.00f, 0.00f };
	FrameBg                     = { 0.12f, 0.12f, 0.13f, 1.00f };
	FrameBgHovered              = { 0.12f, 0.12f, 0.13f, 1.00f };
	FrameBgActive               = { 0.12f, 0.12f, 0.13f, 1.00f };
	TitleBg                     = { 0.01f, 0.02f, 0.03f, 1.00f };
	TitleBgActive               = { 0.01f, 0.02f, 0.03f, 1.00f };
	TitleBgCollapsed            = { 0.01f, 0.02f, 0.03f, 1.00f };
	MenuBarBg                   = { 0.01f, 0.02f, 0.03f, 1.00f };
	ScrollbarBg                 = { 0.07f, 0.07f, 0.07f, 1.00f };
	ScrollbarGrab               = { 0.31f, 0.31f, 0.32f, 1.00f };
	ScrollbarGrabHovered        = { 0.41f, 0.41f, 0.42f, 1.00f };
	ScrollbarGrabActive         = { 0.51f, 0.51f, 0.53f, 1.00f };
	CheckMark                   = { 0.44f, 0.44f, 0.47f, 1.00f };
	SliderGrab                  = { 0.44f, 0.44f, 0.47f, 1.00f };
	SliderGrabActive            = { 0.59f, 0.59f, 0.61f, 1.00f };
	Button                      = { 0.20f, 0.20f, 0.22f, 1.00f };
	ButtonHovered               = { 0.44f, 0.44f, 0.47f, 1.00f };
	ButtonActive                = { 0.59f, 0.59f, 0.61f, 1.00f };
	Header                      = { 0.10f, 0.10f, 0.11f, 1.00f };
	HeaderHovered               = { 0.15f, 0.15f, 0.17f, 1.00f };
	HeaderActive                = { 0.20f, 0.20f, 0.22f, 1.00f };
	Separator                   = { 1.00f, 1.00f, 1.00f, 0.20f };
	SeparatorHovered            = { 0.44f, 0.44f, 0.47f, 0.39f };
	SeparatorActive             = { 0.44f, 0.44f, 0.47f, 0.59f };
	ResizeGrip                  = { 0.26f, 0.59f, 0.98f, 0.00f };
	ResizeGripHovered           = { 0.26f, 0.59f, 0.98f, 0.00f };
	ResizeGripActive            = { 0.26f, 0.59f, 0.98f, 0.00f };
	InputTextCursor             = { 1.00f, 1.00f, 1.00f, 1.00f };
	Tab                         = { 0.10f, 0.10f, 0.11f, 1.00f };
	TabHovered                  = { 0.15f, 0.15f, 0.17f, 1.00f };
	TabSelected                 = { 0.20f, 0.20f, 0.22f, 1.00f };
	TabSelectedOverline         = { 0.20f, 0.20f, 0.22f, 1.00f };
	TabDimmed                   = { 0.10f, 0.10f, 0.11f, 1.00f };
	TabDimmedSelected           = { 0.15f, 0.15f, 0.17f, 1.00f };
	TabDimmedSelectedOverline   = { 0.15f, 0.15f, 0.17f, 1.00f };
	DockingPreview              = { 0.91f, 0.62f, 0.00f, 0.78f };
	DockingEmptyBg              = { 0.20f, 0.20f, 0.20f, 1.00f };
	PlotLines                   = { 0.96f, 0.96f, 0.99f, 1.00f };
	PlotLinesHovered            = { 0.12f, 1.00f, 0.12f, 1.00f };
	PlotHistogram               = { 0.96f, 0.96f, 0.99f, 1.00f };
	PlotHistogramHovered        = { 0.12f, 1.00f, 0.12f, 1.00f };
	TableHeaderBg               = { 0.06f, 0.07f, 0.08f, 1.00f };
	TableBorderStrong           = { 0.10f, 0.10f, 0.12f, 1.00f };
	TableBorderLight            = { 0.07f, 0.07f, 0.09f, 1.00f };
	TableRowBg                  = { 0.00f, 0.00f, 0.00f, 0.00f };
	TableRowBgAlt               = { 1.00f, 1.00f, 1.00f, 0.04f };
	TextLink                    = { 0.26f, 0.59f, 0.98f, 1.00f };
	TextSelectedBg              = { 0.26f, 0.59f, 0.98f, 0.35f };
	TreeLines                   = { 0.50f, 0.50f, 0.50f, 1.00f };
	DragDropTarget              = { 0.91f, 0.62f, 0.00f, 1.00f };
	DragDropTargetBg            = { 0.26f, 0.59f, 0.98f, 0.20f };
	UnsavedMarker               = { 0.26f, 0.99f, 0.26f, 1.00f };
	NavCursor                   = { 0.26f, 0.59f, 0.98f, 1.00f };
	NavWindowingHighlight       = { 1.00f, 1.00f, 1.00f, 0.70f };
	NavWindowingDimBg           = { 0.80f, 0.80f, 0.80f, 0.20f };
	ModalWindowDimBg            = { 0.80f, 0.80f, 0.80f, 0.35f };

	WindowPadding    = { 15.0f, 15.0f };
	WindowRounding   = 0.0f;
	WindowBorderSize = 1.0f;
	FramePadding     = { 5.0f, 5.0f };
	FrameRounding    = 0.0f;
	FrameBorderSize  = 0.0f;
	ItemSpacing      = { 12.0f, 8.0f };
	ItemInnerSpacing = { 8.0f, 6.0f };
	IndentSpacing    = 25.0f;
	ScrollbarSize    = 15.0f;
	ScrollbarRounding= 0.0f;
	GrabMinSize      = 5.0f;
	GrabRounding     = 0.0f;
	TabRounding      = 0.0f;
	ChildRounding    = 0.0f;
	PopupRounding    = 0.0f;
	PopupBorderSize  = 1.0f;

	SetSemanticDefaults();
}

void OvUI::Styling::Style::ApplyToImGui(float p_scale)
{
	ImGuiStyle style;

	style.Colors[ImGuiCol_Text]                     = ToImVec4(Text);
	style.Colors[ImGuiCol_TextDisabled]             = ToImVec4(TextDisabled);
	style.Colors[ImGuiCol_WindowBg]                 = ToImVec4(WindowBg);
	style.Colors[ImGuiCol_ChildBg]                  = ToImVec4(ChildBg);
	style.Colors[ImGuiCol_PopupBg]                  = ToImVec4(PopupBg);
	style.Colors[ImGuiCol_Border]                   = ToImVec4(Border);
	style.Colors[ImGuiCol_BorderShadow]             = ToImVec4(BorderShadow);
	style.Colors[ImGuiCol_FrameBg]                  = ToImVec4(FrameBg);
	style.Colors[ImGuiCol_FrameBgHovered]           = ToImVec4(FrameBgHovered);
	style.Colors[ImGuiCol_FrameBgActive]            = ToImVec4(FrameBgActive);
	style.Colors[ImGuiCol_TitleBg]                  = ToImVec4(TitleBg);
	style.Colors[ImGuiCol_TitleBgActive]            = ToImVec4(TitleBgActive);
	style.Colors[ImGuiCol_TitleBgCollapsed]         = ToImVec4(TitleBgCollapsed);
	style.Colors[ImGuiCol_MenuBarBg]                = ToImVec4(MenuBarBg);
	style.Colors[ImGuiCol_ScrollbarBg]              = ToImVec4(ScrollbarBg);
	style.Colors[ImGuiCol_ScrollbarGrab]            = ToImVec4(ScrollbarGrab);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]     = ToImVec4(ScrollbarGrabHovered);
	style.Colors[ImGuiCol_ScrollbarGrabActive]      = ToImVec4(ScrollbarGrabActive);
	style.Colors[ImGuiCol_CheckMark]                = ToImVec4(CheckMark);
	style.Colors[ImGuiCol_SliderGrab]               = ToImVec4(SliderGrab);
	style.Colors[ImGuiCol_SliderGrabActive]         = ToImVec4(SliderGrabActive);
	style.Colors[ImGuiCol_Button]                   = ToImVec4(Button);
	style.Colors[ImGuiCol_ButtonHovered]            = ToImVec4(ButtonHovered);
	style.Colors[ImGuiCol_ButtonActive]             = ToImVec4(ButtonActive);
	style.Colors[ImGuiCol_Header]                   = ToImVec4(Header);
	style.Colors[ImGuiCol_HeaderHovered]            = ToImVec4(HeaderHovered);
	style.Colors[ImGuiCol_HeaderActive]             = ToImVec4(HeaderActive);
	style.Colors[ImGuiCol_Separator]                = ToImVec4(Separator);
	style.Colors[ImGuiCol_SeparatorHovered]         = ToImVec4(SeparatorHovered);
	style.Colors[ImGuiCol_SeparatorActive]          = ToImVec4(SeparatorActive);
	style.Colors[ImGuiCol_ResizeGrip]               = ToImVec4(ResizeGrip);
	style.Colors[ImGuiCol_ResizeGripHovered]        = ToImVec4(ResizeGripHovered);
	style.Colors[ImGuiCol_ResizeGripActive]         = ToImVec4(ResizeGripActive);
	style.Colors[ImGuiCol_InputTextCursor]          = ToImVec4(InputTextCursor);
	style.Colors[ImGuiCol_TabHovered]               = ToImVec4(TabHovered);
	style.Colors[ImGuiCol_Tab]                      = ToImVec4(Tab);
	style.Colors[ImGuiCol_TabSelected]              = ToImVec4(TabSelected);
	style.Colors[ImGuiCol_TabSelectedOverline]      = ToImVec4(TabSelectedOverline);
	style.Colors[ImGuiCol_TabDimmed]                = ToImVec4(TabDimmed);
	style.Colors[ImGuiCol_TabDimmedSelected]        = ToImVec4(TabDimmedSelected);
	style.Colors[ImGuiCol_TabDimmedSelectedOverline]= ToImVec4(TabDimmedSelectedOverline);
	style.Colors[ImGuiCol_DockingPreview]           = ToImVec4(DockingPreview);
	style.Colors[ImGuiCol_DockingEmptyBg]           = ToImVec4(DockingEmptyBg);
	style.Colors[ImGuiCol_PlotLines]                = ToImVec4(PlotLines);
	style.Colors[ImGuiCol_PlotLinesHovered]         = ToImVec4(PlotLinesHovered);
	style.Colors[ImGuiCol_PlotHistogram]            = ToImVec4(PlotHistogram);
	style.Colors[ImGuiCol_PlotHistogramHovered]     = ToImVec4(PlotHistogramHovered);
	style.Colors[ImGuiCol_TableHeaderBg]            = ToImVec4(TableHeaderBg);
	style.Colors[ImGuiCol_TableBorderStrong]        = ToImVec4(TableBorderStrong);
	style.Colors[ImGuiCol_TableBorderLight]         = ToImVec4(TableBorderLight);
	style.Colors[ImGuiCol_TableRowBg]               = ToImVec4(TableRowBg);
	style.Colors[ImGuiCol_TableRowBgAlt]            = ToImVec4(TableRowBgAlt);
	style.Colors[ImGuiCol_TextLink]                 = ToImVec4(TextLink);
	style.Colors[ImGuiCol_TextSelectedBg]           = ToImVec4(TextSelectedBg);
	style.Colors[ImGuiCol_TreeLines]                = ToImVec4(TreeLines);
	style.Colors[ImGuiCol_DragDropTarget]           = ToImVec4(DragDropTarget);
	style.Colors[ImGuiCol_DragDropTargetBg]         = ToImVec4(DragDropTargetBg);
	style.Colors[ImGuiCol_UnsavedMarker]            = ToImVec4(UnsavedMarker);
	style.Colors[ImGuiCol_NavCursor]                = ToImVec4(NavCursor);
	style.Colors[ImGuiCol_NavWindowingHighlight]    = ToImVec4(NavWindowingHighlight);
	style.Colors[ImGuiCol_NavWindowingDimBg]        = ToImVec4(NavWindowingDimBg);
	style.Colors[ImGuiCol_ModalWindowDimBg]         = ToImVec4(ModalWindowDimBg);

	style.WindowPadding     = Internal::Converter::ToImVec2(WindowPadding);
	style.WindowRounding    = WindowRounding;
	style.WindowBorderSize  = WindowBorderSize;
	style.FramePadding      = Internal::Converter::ToImVec2(FramePadding);
	style.FrameRounding     = FrameRounding;
	style.FrameBorderSize   = FrameBorderSize;
	style.ItemSpacing       = Internal::Converter::ToImVec2(ItemSpacing);
	style.ItemInnerSpacing  = Internal::Converter::ToImVec2(ItemInnerSpacing);
	style.IndentSpacing     = IndentSpacing;
	style.ScrollbarSize     = ScrollbarSize;
	style.ScrollbarRounding = ScrollbarRounding;
	style.GrabMinSize       = GrabMinSize;
	style.GrabRounding      = GrabRounding;
	style.TabRounding       = TabRounding;
	style.ChildRounding     = ChildRounding;
	style.PopupRounding     = PopupRounding;
	style.PopupBorderSize   = PopupBorderSize;

	style.FontScaleMain = p_scale;
	style.ScaleAllSizes(p_scale);
	ImGui::GetStyle() = style;
}
