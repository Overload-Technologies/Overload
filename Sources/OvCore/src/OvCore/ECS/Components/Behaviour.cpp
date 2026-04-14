/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>
#include <limits>
#include <utility>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/Behaviour.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/GUIHelpers.h>
#include <OvCore/Scripting/ScriptEngine.h>

#include <OvTools/Utils/PathParser.h>

#include <OvDebug/Logger.h>

#include <OvCore/SceneSystem/SceneManager.h>

#include <OvUI/Plugins/DataDispatcher.h>
#include <OvUI/Plugins/DDTarget.h>
#include <OvUI/Widgets/Drags/DragSingleScalar.h>
#include <OvUI/Widgets/InputFields/ActorField.h>
#include <OvUI/Widgets/InputFields/AssetField.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Dummy.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/TreeNode.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Texts/TextColored.h>

OvCore::ECS::Components::Behaviour::Behaviour(ECS::Actor& p_owner, const std::string& p_name) :
	name(p_name), AComponent(p_owner)
{
	OVSERVICE(Scripting::ScriptEngine).AddBehaviour(*this);
}

OvCore::ECS::Components::Behaviour::~Behaviour()
{
	OVSERVICE(Scripting::ScriptEngine).RemoveBehaviour(*this);
}

std::string OvCore::ECS::Components::Behaviour::GetName()
{
	return "Behaviour";
}

std::string OvCore::ECS::Components::Behaviour::GetTypeName()
{
	return std::string{ComponentTraits<Behaviour>::Name};
}

void OvCore::ECS::Components::Behaviour::SetScript(std::unique_ptr<Scripting::Script> &&p_scriptContext)
{
	m_script = std::move(p_scriptContext);

	if (!m_script || !m_script->IsValid())
	{
		m_scriptProperties.clear();
		m_scriptDefaults.clear();
		m_unlockedProperties.clear();
		return;
	}

	m_scriptDefaults = m_script->GetDefaultProperties();

	auto old = std::exchange(m_scriptProperties, {});
	auto oldUnlocked = std::exchange(m_unlockedProperties, {});

	for (const auto& [key, defaultVal] : m_scriptDefaults)
	{
		const bool wasUnlocked = oldUnlocked.contains(key);
		auto it = old.find(key);
		const bool canPreserve = wasUnlocked && it != old.end() && it->second.index() == defaultVal.index();

		m_scriptProperties[key] = canPreserve ? std::move(it->second) : defaultVal;

		if (wasUnlocked)
			m_unlockedProperties.insert(key);
	}

	// Push user overrides back into the live script.
	for (const auto& [key, val] : m_scriptProperties)
		m_script->SetProperty(key, val);
}

OvTools::Utils::OptRef<OvCore::Scripting::Script> OvCore::ECS::Components::Behaviour::GetScript()
{
	if (m_script)
	{
		return { *m_script };
	}

	return std::nullopt;
}

void OvCore::ECS::Components::Behaviour::RemoveScript()
{
	m_script.reset();
}

void OvCore::ECS::Components::Behaviour::OnAwake()
{
	OVSERVICE(Scripting::ScriptEngine).OnAwake(*this);
}

void OvCore::ECS::Components::Behaviour::OnStart()
{
	OVSERVICE(Scripting::ScriptEngine).OnStart(*this);
}

void OvCore::ECS::Components::Behaviour::OnEnable()
{
	OVSERVICE(Scripting::ScriptEngine).OnEnable(*this);
}

void OvCore::ECS::Components::Behaviour::OnDisable()
{
	OVSERVICE(Scripting::ScriptEngine).OnDisable(*this);
}

void OvCore::ECS::Components::Behaviour::OnDestroy()
{
	OVSERVICE(Scripting::ScriptEngine).OnDestroy(*this);
}

void OvCore::ECS::Components::Behaviour::OnUpdate(float p_deltaTime)
{
	OVSERVICE(Scripting::ScriptEngine).OnUpdate(*this, p_deltaTime);
}

void OvCore::ECS::Components::Behaviour::OnFixedUpdate(float p_deltaTime)
{
	OVSERVICE(Scripting::ScriptEngine).OnFixedUpdate(*this, p_deltaTime);
}

void OvCore::ECS::Components::Behaviour::OnLateUpdate(float p_deltaTime)
{
	OVSERVICE(Scripting::ScriptEngine).OnLateUpdate(*this, p_deltaTime);
}

void OvCore::ECS::Components::Behaviour::OnCollisionEnter(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnCollisionEnter(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnCollisionStay(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnCollisionStay(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnCollisionExit(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnCollisionExit(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnTriggerEnter(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnTriggerEnter(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnTriggerStay(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnTriggerStay(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnTriggerExit(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnTriggerExit(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	if (m_unlockedProperties.empty()) return;

	tinyxml2::XMLNode* propsNode = p_doc.NewElement("script_properties");
	p_node->InsertEndChild(propsNode);

	for (const auto& [fieldKey, fieldValue] : m_scriptProperties)
	{
		if (!m_unlockedProperties.contains(fieldKey)) continue;

		tinyxml2::XMLElement* elem = p_doc.NewElement(fieldKey.c_str());

		std::visit([elem]<typename T>(const T& v) {
			if constexpr (std::is_same_v<T, bool>)
				{ elem->SetAttribute("type", "bool");   elem->SetText(v); }
			else if constexpr (std::is_same_v<T, double>)
				{ elem->SetAttribute("type", "number"); elem->SetText(v); }
			else if constexpr (std::is_same_v<T, OvCore::Scripting::AssetRef>)
				{ elem->SetAttribute("type", "asset"); elem->SetAttribute("assetType", OvTools::Utils::PathParser::FileTypeToString(v.fileType).c_str()); elem->SetText(v.path.c_str()); }
			else if constexpr (std::is_same_v<T, OvCore::Scripting::ActorRef>)
				{ elem->SetAttribute("type", "actor"); elem->SetText(std::to_string(v.guid).c_str()); }
			else
				{ elem->SetAttribute("type", "string"); elem->SetText(v.c_str()); }
		}, fieldValue);

		propsNode->InsertEndChild(elem);
	}
}

void OvCore::ECS::Components::Behaviour::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	const tinyxml2::XMLElement* propsNode = p_node->FirstChildElement("script_properties");
	if (!propsNode) return;

	for (const tinyxml2::XMLElement* elem = propsNode->FirstChildElement(); elem; elem = elem->NextSiblingElement())
	{
		const char* const name = elem->Name();
		const char* const type = elem->Attribute("type");
		if (!name || !type) continue;

		auto it = m_scriptProperties.find(name);
		if (it == m_scriptProperties.end()) continue;

		const std::string_view typeStr{type};
		auto& val = it->second;

		if (typeStr == "bool" && std::holds_alternative<bool>(val))
		{
			bool v = false;
			elem->QueryBoolText(&v);
			val = v;
		}
		else if (typeStr == "number" && std::holds_alternative<double>(val))
		{
			double v = 0.0;
			elem->QueryDoubleText(&v);
			val = v;
		}
		else if (typeStr == "string" && std::holds_alternative<std::string>(val))
			val = elem->GetText() ? elem->GetText() : "";
		else if (typeStr == "asset" && std::holds_alternative<OvCore::Scripting::AssetRef>(val))
		{
			const char* assetTypeStr = elem->Attribute("assetType");
			val = OvCore::Scripting::AssetRef{
				assetTypeStr ? OvTools::Utils::PathParser::StringToFileType(assetTypeStr)
				             : std::get<OvCore::Scripting::AssetRef>(val).fileType,
				elem->GetText() ? elem->GetText() : ""
			};
		}
		else if (typeStr == "actor" && std::holds_alternative<OvCore::Scripting::ActorRef>(val))
		{
			uint64_t guid = 0;
			if (const char* text = elem->GetText())
				guid = std::stoull(text);
			val = OvCore::Scripting::ActorRef{guid};
		}
		else
			continue;

		m_unlockedProperties.insert(name);

		if (m_script)
			m_script->SetProperty(name, val);
	}
}

void OvCore::ECS::Components::Behaviour::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	using namespace OvCore::Helpers;

	if (!m_script)
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("No scripting context", OvUI::Types::Color::White);
		p_root.CreateWidget<OvUI::Widgets::Layout::Dummy>();
	}
	else if (m_script->IsValid())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Ready", OvUI::Types::Color::Green);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Script properties will appear below", OvUI::Types::Color::White);

		for (const auto& [fieldKey, fieldValue] : m_scriptProperties)
		{
			const bool isUnlocked = m_unlockedProperties.contains(fieldKey);

			std::visit([&, key = fieldKey, unlocked = isUnlocked]<typename T>(const T&) {
				auto getter = [this, key]() -> T {
					if (auto live = m_script->GetProperty(key))
						if (auto* val = std::get_if<T>(&*live))
							return *val;
					auto it = m_scriptProperties.find(key);
					return it != m_scriptProperties.end() ? std::get<T>(it->second) : T{};
				};
				auto setter = [this, key](T newVal) {
					m_scriptProperties[key] = newVal;
					if (m_script) m_script->SetProperty(key, std::move(newVal));
				};

				// Label row: [unlock checkbox] [field title], grouped together
				auto& labelGroup = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();
				auto& unlockBox = labelGroup.CreateWidget<OvUI::Widgets::Selection::CheckBox>(unlocked);
				unlockBox.lineBreak = false;
				labelGroup.CreateWidget<OvUI::Widgets::Texts::TextColored>(key, GUIDrawer::TitleColor);

				// Input widget on the row below
				OvUI::Widgets::AWidget* inputPtr = nullptr;
				if constexpr (std::is_same_v<T, bool>)
				{
					auto& w = p_root.CreateWidget<OvUI::Widgets::Selection::CheckBox>();
					auto& d = w.template AddPlugin<OvUI::Plugins::DataDispatcher<bool>>();
					d.RegisterGatherer([getter]() { bool v = getter(); return reinterpret_cast<bool&>(v); });
					d.RegisterProvider([setter](bool v) { setter(reinterpret_cast<bool&>(v)); });
					inputPtr = &w;
				}
				else if constexpr (std::is_same_v<T, double>)
				{
					auto& w = p_root.CreateWidget<OvUI::Widgets::Drags::DragSingleScalar<double>>(
						GUIDrawer::GetDataType<double>(),
						std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(),
						static_cast<double>(0), 1.f, "", GUIDrawer::GetFormat<double>()
					);
					auto& d = w.template AddPlugin<OvUI::Plugins::DataDispatcher<double>>();
					d.RegisterGatherer(getter);
					d.RegisterProvider(setter);
					inputPtr = &w;
				}
				else if constexpr (std::is_same_v<T, OvCore::Scripting::AssetRef>)
				{
					const auto fileType = getter().fileType;

					auto pathGatherer = [getter]() { return getter().path; };
					auto pathProvider = [setter, getter](std::string newPath) {
						auto ref = getter();
						ref.path = std::move(newPath);
						setter(std::move(ref));
					};

					// Create the AssetField directly (without a GUIDrawer title, since the
					// label row above already serves that purpose).
					auto& w = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(pathGatherer());
					w.iconTextureID = OvCore::Helpers::GUIHelpers::GetIconForFileType(fileType);
					w.displayFormatter = &OvCore::Helpers::GUIDrawer::GetAssetDisplayName;
					w.tooltipFormatter = &OvCore::Helpers::GUIDrawer::GetAssetTooltip;

					auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::AssetField>(&w, [](void*) {});
					w.template AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>().RegisterGatherer(pathGatherer);

					w.template AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
						[widgetPtr, pathProvider, fileType](auto p_receivedData)
					{
						const bool typeMatch = fileType == OvTools::Utils::PathParser::EFileType::UNKNOWN
							|| OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == fileType;
						if (typeMatch)
						{
							widgetPtr->content = p_receivedData.first;
							pathProvider(p_receivedData.first);
						}
					};

					auto token = std::make_shared<bool>(true);
					w.ClickedEvent += [widgetPtr, pathProvider, fileType, token]()
					{
						std::weak_ptr<bool> weak = token;
						OvCore::Helpers::GUIHelpers::OpenAssetPicker(fileType,
							[widgetPtr, pathProvider, weak](const std::string& p_path)
							{
								if (!weak.expired())
								{
									widgetPtr->content = p_path;
									pathProvider(p_path);
								}
							}, true, true);
					};

					w.DoubleClickedEvent += [widgetPtr] { OvCore::Helpers::GUIHelpers::Open(widgetPtr->content); };

					inputPtr = &w;
				}
				else if constexpr (std::is_same_v<T, OvCore::Scripting::ActorRef>)
				{
					// Determine display name and tooltip from the scene.
					auto resolveDisplayName = [](uint64_t guid) -> std::string {
						if (guid == 0) return "";
						auto* scene = OVSERVICE(OvCore::SceneSystem::SceneManager).GetCurrentScene();
						auto* actor = scene ? scene->FindActorByGUID(guid) : nullptr;
						return actor ? actor->GetName() : "(Missing)";
					};

					const OvCore::Scripting::ActorRef initial = getter();
					auto& w = p_root.CreateWidget<OvUI::Widgets::InputFields::ActorField>(initial.guid);
					w.iconTextureID = OvCore::Helpers::GUIHelpers::GetActorIconID();
					w.displayFormatter = [resolveDisplayName](uint64_t guid) { return resolveDisplayName(guid); };
					w.tooltipFormatter = [](uint64_t guid) -> std::string {
						return guid != 0 ? std::format("GUID: {:016X}", guid) : "";
					};

					auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::ActorField>(&w, [](void*) {});

					w.template AddPlugin<OvUI::Plugins::DataDispatcher<uint64_t>>().RegisterGatherer(
						[getter]() -> uint64_t { return getter().guid; }
					);

					// Drag-drop: accept an actor dropped from the hierarchy.
					using ActorPayload = std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*>;
					w.template AddPlugin<OvUI::Plugins::DDTarget<ActorPayload>>("Actor").DataReceivedEvent +=
						[widgetPtr, setter](ActorPayload p_payload)
					{
						if (p_payload.first)
						{
							widgetPtr->content = p_payload.first->GetGUID();
							setter(OvCore::Scripting::ActorRef{widgetPtr->content});
						}
					};

					// Double-click: select the referenced actor in the hierarchy.
					w.DoubleClickedEvent += [widgetPtr]()
					{
						OvCore::Helpers::GUIHelpers::SelectActor(widgetPtr->content);
					};

					// Picker button: list all scene actors.
					auto token = std::make_shared<bool>(true);
					w.ClickedEvent += [widgetPtr, setter, token]()
					{
						auto* scene = OVSERVICE(OvCore::SceneSystem::SceneManager).GetCurrentScene();
						if (!scene) return;

						std::weak_ptr<bool> weak = token;
						OvCore::Helpers::GUIHelpers::PickerItemList items;
						items.Add({ "__none__", "None", "Clear the current selection", 0u,
							[widgetPtr, setter, weak]()
							{
								if (weak.expired()) return;
								widgetPtr->content = 0;
								setter(OvCore::Scripting::ActorRef{0});
							}
						});
						for (auto& actor : scene->GetActors())
						{
							const uint64_t guid = actor->GetGUID();
							items.Add({
								std::to_string(guid),
								actor->GetName(),
								"GUID: " + std::format("{:016X}", guid),
								OvCore::Helpers::GUIHelpers::GetActorIconID(),
								[widgetPtr, setter, guid, weak]()
								{
									if (!weak.expired())
									{
										widgetPtr->content = guid;
										setter(OvCore::Scripting::ActorRef{guid});
									}
								}
							});
						}

						OvCore::Helpers::GUIHelpers::OpenPicker(std::move(items), "Select Actor");
					};

					inputPtr = &w;
				}
				else
				{
					auto& w = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
					auto& d = w.template AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
					d.RegisterGatherer(getter);
					d.RegisterProvider(setter);
					inputPtr = &w;
				}

				auto& inputWidget = *inputPtr;
				inputWidget.disabled = !unlocked;

				unlockBox.ValueChangedEvent += [this, key, &inputWidget](bool checked) {
					if (checked)
					{
						m_unlockedProperties.insert(key);
					}
					else
					{
						m_unlockedProperties.erase(key);
						if (auto it = m_scriptDefaults.find(key); it != m_scriptDefaults.end())
						{
							m_scriptProperties[key] = it->second;
							if (m_script) m_script->SetProperty(key, it->second);
						}
					}
					inputWidget.disabled = !checked;
				};
			}, fieldValue);
		}
	}
	else
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Invalid Script", OvUI::Types::Color::Red);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Check the console for more information.", OvUI::Types::Color::White);
	}
}
