/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <string>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/UIInspectorUtils.h>
#include <OvCore/ECS/Components/UI/UITransformResolver.h>
#include <OvUI/Plugins/DataDispatcher.h>
#include <OvUI/Widgets/Texts/TextColored.h>

void OvCore::ECS::Components::UI::UIInspectorUtils::DrawCanvasRequirement(
	OvUI::Internal::WidgetContainer& p_root,
	const ECS::Actor& p_owner
)
{
	// Gathered every frame, so adding or removing a parent canvas is reported without a panel refresh
	const auto hasCanvas = [&p_owner]
	{
		return OvCore::ECS::Components::UI::UITransformResolver::HasActiveUIData(p_owner);
	};

	// The inspector lays components out in two columns, so the status lands next to its details,
	// the way skinned mesh renderers report theirs
	auto& canvasStatus = p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>();
	canvasStatus.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>().RegisterGatherer([&canvasStatus, hasCanvas]
	{
		const bool ready = hasCanvas();
		canvasStatus.color = ready ? OVUI_STYLE(Success) : OVUI_STYLE(Danger);
		return ready ? std::string{ "Ready" } : std::string{ "Error" };
	});

	auto& canvasDiagnostic = p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("", OVUI_STYLE(TextDisabled));
	canvasDiagnostic.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>().RegisterGatherer([hasCanvas]
	{
		return hasCanvas() ? std::string{ "Parent canvas found" } : std::string{ "No parent canvas found" };
	});
}
