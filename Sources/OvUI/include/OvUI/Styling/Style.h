/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector2.h>
#include <OvUI/Types/Color.h>

/**
* Retrieve a live const-reference to a style color field.
* Example: ColorEffector::Ref(OVUI_STYLE(DangerButton))
*/
#define OVUI_STYLE(field) (OvUI::Styling::Style::Get().field)

namespace OvUI::Styling
{
	/**
	* Singleton holding all UI style data (colors and dimensions).
	*
	* Apply*Style() methods update the stored data without touching ImGui.
	* ApplyToImGui(scale) syncs the current data to the active ImGui context.
	*
	* The constructor initialises data via ApplyDefaultDarkStyle() (no ImGui calls).
	* UIManager is responsible for calling ApplyToImGui() after the ImGui context is ready.
	*/
	class Style
	{
	public:
		Style(const Style&) = delete;
		Style& operator=(const Style&) = delete;

		static Style& Get();

		// ---- Preset methods (data only — do NOT call ImGui) ----
		void ApplyImClassicStyle();
		void ApplyImDarkStyle();
		void ApplyImLightStyle();
		void ApplyDuneDarkStyle();
		void ApplyDefaultDarkStyle();
		void ApplyEvenDarkerStyle();

		/**
		* Sync current style data to the active ImGui context.
		* Builds a temporary ImGuiStyle, scales it by p_scale, then assigns it.
		* Must only be called when an ImGui context exists.
		*/
		void ApplyToImGui(float p_scale = 1.0f);

		// ---- ImGui colors — 1:1 with ImGuiCol_* ----
		Types::Color Text;
		Types::Color TextDisabled;
		Types::Color WindowBg;
		Types::Color ChildBg;
		Types::Color PopupBg;
		Types::Color Border;
		Types::Color BorderShadow;
		Types::Color FrameBg;
		Types::Color FrameBgHovered;
		Types::Color FrameBgActive;
		Types::Color TitleBg;
		Types::Color TitleBgActive;
		Types::Color TitleBgCollapsed;
		Types::Color MenuBarBg;
		Types::Color ScrollbarBg;
		Types::Color ScrollbarGrab;
		Types::Color ScrollbarGrabHovered;
		Types::Color ScrollbarGrabActive;
		Types::Color CheckMark;
		Types::Color SliderGrab;
		Types::Color SliderGrabActive;
		Types::Color Button;
		Types::Color ButtonHovered;
		Types::Color ButtonActive;
		Types::Color Header;
		Types::Color HeaderHovered;
		Types::Color HeaderActive;
		Types::Color Separator;
		Types::Color SeparatorHovered;
		Types::Color SeparatorActive;
		Types::Color ResizeGrip;
		Types::Color ResizeGripHovered;
		Types::Color ResizeGripActive;
		Types::Color InputTextCursor;
		Types::Color TabHovered;
		Types::Color Tab;
		Types::Color TabSelected;
		Types::Color TabSelectedOverline;
		Types::Color TabDimmed;
		Types::Color TabDimmedSelected;
		Types::Color TabDimmedSelectedOverline;
		Types::Color DockingPreview;
		Types::Color DockingEmptyBg;
		Types::Color PlotLines;
		Types::Color PlotLinesHovered;
		Types::Color PlotHistogram;
		Types::Color PlotHistogramHovered;
		Types::Color TableHeaderBg;
		Types::Color TableBorderStrong;
		Types::Color TableBorderLight;
		Types::Color TableRowBg;
		Types::Color TableRowBgAlt;
		Types::Color TextLink;
		Types::Color TextSelectedBg;
		Types::Color TreeLines;
		Types::Color DragDropTarget;
		Types::Color DragDropTargetBg;
		Types::Color UnsavedMarker;
		Types::Color NavCursor;
		Types::Color NavWindowingHighlight;
		Types::Color NavWindowingDimBg;
		Types::Color ModalWindowDimBg;

		// ---- ImGuiStyle dimensions ----
		OvMaths::FVector2 WindowPadding;
		float WindowRounding           = 0.0f;
		float WindowBorderSize         = 1.0f;
		OvMaths::FVector2 FramePadding;
		float FrameRounding            = 0.0f;
		float FrameBorderSize          = 0.0f;
		OvMaths::FVector2 ItemSpacing;
		OvMaths::FVector2 ItemInnerSpacing;
		float IndentSpacing            = 21.0f;
		float ScrollbarSize            = 14.0f;
		float ScrollbarRounding        = 9.0f;
		float GrabMinSize              = 4.0f;
		float GrabRounding             = 0.0f;
		float TabRounding              = 4.0f;
		float ChildRounding            = 0.0f;
		float PopupRounding            = 0.0f;
		float PopupBorderSize          = 1.0f;

		// ---- Semantic colors (Bootstrap-style) ----
		// These remain consistent across all theme presets.
		Types::Color PrimaryButton;
		Types::Color PrimaryButtonHovered;
		Types::Color PrimaryButtonActive;

		Types::Color SuccessButton;
		Types::Color SuccessButtonHovered;
		Types::Color SuccessButtonActive;

		Types::Color DangerButton;
		Types::Color DangerButtonHovered;
		Types::Color DangerButtonActive;

		Types::Color WarningButton;
		Types::Color WarningButtonHovered;
		Types::Color WarningButtonActive;

		// ---- Log level colors ----
		Types::Color LogDefault;
		Types::Color LogInfo;
		Types::Color LogWarning;
		Types::Color LogError;

		// ---- Inspector / script-status colors ----
		Types::Color InspectorTitle;
		Types::Color StatusDefault;
		Types::Color StatusReady;
		Types::Color StatusError;

	private:
		Style();

		// Populates all 62 ImGui color members + dimension members from a pre-built ImGuiStyle.
		// Does NOT set semantic colors — call SetSemanticDefaults() after this.
		void PopulateFromImGuiStyle(const struct ImGuiStyle& p_style);

		// Sets semantic colors for dark-background themes.
		void SetSemanticDefaults();

		// Overrides semantic colors for light-background themes.
		void SetSemanticLightTheme();
	};
}
