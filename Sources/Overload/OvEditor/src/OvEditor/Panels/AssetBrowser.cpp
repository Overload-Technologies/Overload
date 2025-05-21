/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <tinyxml2.h>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

#include <OvDebug/Logger.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Panels/AssetBrowser.h>
#include <OvEditor/Panels/AssetProperties.h>
#include <OvEditor/Panels/AssetView.h>
#include <OvEditor/Panels/MaterialEditor.h>

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/String.h>
#include <OvTools/Utils/SystemCalls.h>

#include <OvUI/Plugins/ContextualMenu.h>
#include <OvUI/Plugins/DDSource.h>
#include <OvUI/Plugins/DDTarget.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/TextClickable.h>
#include <OvUI/Widgets/Visual/Image.h>
#include <OvUI/Widgets/Visual/Separator.h>

#include <OvWindowing/Dialogs/MessageBox.h>
#include <OvWindowing/Dialogs/OpenFileDialog.h>
#include <OvWindowing/Dialogs/SaveFileDialog.h>

using namespace OvUI::Panels;
using namespace OvUI::Widgets;

namespace
{
	constexpr std::string_view kAllowedFilenameChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_=+ 0123456789()[]";

	template<typename ResourceManager>
	auto& GetResource(const std::string& p_path, bool p_isEngineResource)
	{
		auto resource = OvCore::Global::ServiceLocator::Get<ResourceManager>()[EDITOR_EXEC(GetResourcePath(p_path, p_isEngineResource))];
		OVASSERT(resource, "Resource not found");
		return *resource;
	}

	void OpenInAssetView(auto& p_resource)
	{
		auto& assetView = EDITOR_PANEL(OvEditor::Panels::AssetView, "Asset View");
		assetView.SetResource(OvEditor::Panels::AssetView::ViewableResource{ &p_resource });
		assetView.Open();
		assetView.Focus();
	}

	void OpenInMaterialEditor(auto& p_resource)
	{
		auto& materialEditor = EDITOR_PANEL(OvEditor::Panels::MaterialEditor, "Material Editor");
		materialEditor.SetTarget(p_resource);
		materialEditor.Open();
		materialEditor.Focus();
	}

	std::filesystem::path GetAssociatedMetaFile(const std::filesystem::path& p_assetPath)
	{
		return p_assetPath.string() + ".meta";
	}

	void RenameAsset(const std::filesystem::path& p_prev, const std::filesystem::path& p_new)
	{
		std::filesystem::rename(p_prev, p_new);

		if (const auto previousMetaPath = GetAssociatedMetaFile(p_prev); std::filesystem::exists(previousMetaPath))
		{
			if (const auto newMetaPath = GetAssociatedMetaFile(p_new); !std::filesystem::exists(newMetaPath))
			{
				std::filesystem::rename(previousMetaPath, newMetaPath);
			}
			else
			{
				OVLOG_ERROR(std::format("{} is already existing, .meta creation failed", newMetaPath.string()));
			}
		}
	}

	void RemoveAsset(const std::string& p_toDelete)
	{
		std::filesystem::remove(p_toDelete);

		if (const auto metaPath = GetAssociatedMetaFile(p_toDelete); std::filesystem::exists(metaPath))
		{
			std::filesystem::remove(metaPath);
		}
	}

	std::filesystem::path FindAvailableFilePath(const std::filesystem::path& p_path)
	{
		if (!std::filesystem::exists(p_path))
		{
			return p_path;
		}

		// Split the path into directory, filename, and extension
		const std::filesystem::path dir = p_path.parent_path();
		const std::string filename = p_path.stem().string();
		const std::string extension = p_path.extension().string();

		std::optional<std::string> baseName;
		std::optional<uint32_t> increment;

		const std::regex pattern(R"((.*?)(?:-(\d+))?)");
		std::smatch matches;

		if (std::regex_match(filename, matches, pattern))
		{
			baseName = matches[1].str();

			if (matches[2].matched)
			{
				increment = std::atoi(matches[2].str().c_str());
			}
		}

		constexpr uint32_t kMaxAttempts = 256;

		for (uint32_t i = increment.value_or(1); i < kMaxAttempts; ++i)
		{
			const auto newPath = dir / std::format("{}-{}{}", baseName.value_or("new_file"), i, extension);

			if (!std::filesystem::exists(newPath))
			{
				return newPath;
			}
		}

		OVASSERT(false, "Failed to generate a unique file name.");
		return p_path;
	}

	class TexturePreview : public OvUI::Plugins::IPlugin
	{
	private:
		constexpr static float kTexturePreviewSize = 80.0f;

	public:
		TexturePreview() : image(0, { kTexturePreviewSize, kTexturePreviewSize }) { }

		void SetPath(const std::string& p_path)
		{
			texture = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>()[p_path];
		}

		virtual void Execute(OvUI::Plugins::EPluginExecutionContext p_context) override
		{
			if (ImGui::IsItemHovered())
			{
				if (texture)
				{
					image.textureID.id = texture->GetTexture().GetID();
				}

				ImGui::BeginTooltip();
				image.Draw();
				ImGui::EndTooltip();
			}
		}

		OvRendering::Resources::Texture* texture = nullptr;
		OvUI::Widgets::Visual::Image image;
	};

	class BrowserItemContextualMenu : public OvUI::Plugins::ContextualMenu
	{
	public:
		BrowserItemContextualMenu(const std::string p_filePath, bool p_protected = false) : m_protected(p_protected), filePath(p_filePath) {}

		virtual void CreateList()
		{
			if (!m_protected)
			{
				auto& deleteAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Delete");
				deleteAction.ClickedEvent += [this] { DeleteItem(); };

				auto& renameMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Rename to...");

				auto& nameEditor = renameMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				nameEditor.selectAllOnClick = true;

				renameMenu.ClickedEvent +=[this, &nameEditor] {
					nameEditor.content = filePath.stem().string();

					if (!std::filesystem::is_directory(filePath))
					{
						if (size_t pos = nameEditor.content.rfind('.'); pos != std::string::npos)
						{
							nameEditor.content = nameEditor.content.substr(0, pos);
						}
					}
				};

				nameEditor.EnterPressedEvent += [this](std::string p_newName)
				{
					if (!std::filesystem::is_directory(filePath))
					{
						p_newName += filePath.extension().string();
					}

					// Remove non-allowed characters.
					std::erase_if(p_newName, [](char c) {
						return kAllowedFilenameChars.find(c) >= kAllowedFilenameChars.size();
					});

					const std::filesystem::path parentFolder = std::filesystem::path{ filePath }.parent_path();
					const std::filesystem::path newPath = parentFolder / p_newName;
					const std::filesystem::path oldPath = filePath;

					if (filePath != newPath && !std::filesystem::exists(newPath))
					{
						filePath = newPath;
					}

					RenamedEvent.Invoke(oldPath, newPath);
				};
			}
		}

		virtual void Execute(OvUI::Plugins::EPluginExecutionContext p_context) override
		{
			if (m_widgets.size() > 0)
			{
				OvUI::Plugins::ContextualMenu::Execute(p_context);
			}
		}

		virtual void DeleteItem() = 0;

	public:
		bool m_protected;
		std::filesystem::path filePath;
		OvTools::Eventing::Event<std::filesystem::path> DestroyedEvent;
		OvTools::Eventing::Event<std::filesystem::path, std::filesystem::path> RenamedEvent;
	};

	class FolderContextualMenu : public BrowserItemContextualMenu
	{
	public:
		FolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

		void CreateNewShader(const std::string& p_shaderName, std::optional<const std::string_view> p_type)
		{
			const auto finalPath = FindAvailableFilePath(filePath / (p_shaderName + ".ovfx"));

			if (p_type.has_value())
			{
				std::filesystem::copy_file(
					std::filesystem::path(EDITOR_CONTEXT(engineAssetsPath)) /
					"Shaders" /
					std::format("{}.ovfx", p_type.value()),
					finalPath
				);
			}
			else
			{
				// Empty shader.
				std::ofstream outfile(finalPath);
			}

			ItemAddedEvent.Invoke(finalPath.string());
			Close();
		}

		void CreateNewShaderCallback(
			OvUI::Widgets::InputFields::InputText& p_inputText,
			std::optional<const std::string_view> p_type = std::nullopt
		)
		{
			p_inputText.EnterPressedEvent += std::bind(
				&FolderContextualMenu::CreateNewShader,
				this, std::placeholders::_1,
				p_type
			);
		}

		void CreateNewMaterial(
			const std::string& p_materialName,
			std::optional<const std::string_view> p_type,
			std::optional<std::function<void(OvCore::Resources::Material&)>> p_setupCallback
		)
		{
			OvCore::Resources::Material material;

			if (p_type.has_value())
			{
				const std::string shaderPath = std::format(":Shaders\\{}.ovfx", p_type.value());

				if (auto shader = EDITOR_CONTEXT(shaderManager)[shaderPath])
				{
					material.SetShader(shader);
				}
			}

			if (p_setupCallback.has_value())
			{
				p_setupCallback.value()(material);
			}

			const auto finalPath = FindAvailableFilePath(filePath / (p_materialName + ".ovmat"));
			OvCore::Resources::Loaders::MaterialLoader::Save(material, finalPath.string());

			ItemAddedEvent.Invoke(finalPath);

			if (auto instance = EDITOR_CONTEXT(materialManager)[EDITOR_EXEC(GetResourcePath(finalPath.string()))])
			{
				auto& materialEditor = EDITOR_PANEL(OvEditor::Panels::MaterialEditor, "Material Editor");
				OpenInMaterialEditor(*instance);
				OpenInAssetView(*instance);
			}

			Close();
		}

		void CreateNewMaterialCallback(
			OvUI::Widgets::InputFields::InputText& p_inputText,
			std::optional<const std::string_view> p_type = std::nullopt,
			std::optional<std::function<void(OvCore::Resources::Material&)>> p_setupCallback = std::nullopt
		)
		{
			p_inputText.EnterPressedEvent += std::bind(
				&FolderContextualMenu::CreateNewMaterial,
				this, std::placeholders::_1,
				p_type,
				p_setupCallback
			);
		}

		virtual void CreateList() override
		{
			auto& showInExplorer = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Show in explorer");
			showInExplorer.ClickedEvent += [this]
			{
				OvTools::Utils::SystemCalls::ShowInExplorer(filePath.string());
			};

			if (!m_protected)
			{
				auto& importAssetHere = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Import Here...");
				importAssetHere.ClickedEvent += [this]
				{
					if (EDITOR_EXEC(ImportAssetAtLocation(filePath.string())))
					{
						OvUI::Widgets::Layout::TreeNode* pluginOwner = reinterpret_cast<OvUI::Widgets::Layout::TreeNode*>(userData);
						pluginOwner->Close();
						EDITOR_EXEC(DelayAction(std::bind(&OvUI::Widgets::Layout::TreeNode::Open, pluginOwner)));
					}
				};

				auto& createMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Create..");

				auto& createFolderMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Folder");
				auto& createSceneMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Scene");
				auto& createShaderMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Shader");
				auto& createMaterialMenu = createMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Material");

				auto& createEmptyShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Empty");
				auto& createPartialShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Partial");
				auto& createStandardShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Standard template");
				auto& createUnlitShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Unlit template");
				auto& createSkysphereShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Skysphere template");
				auto& createAtmosphereShaderMenu = createShaderMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Atmosphere template");

				auto& createEmptyMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Empty");
				auto& createStandardMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Standard");
				auto& createUnlitMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Unlit");
				auto& createSkysphereMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Skysphere");
				auto& createAtmosphereMaterialMenu = createMaterialMenu.CreateWidget<OvUI::Widgets::Menu::MenuList>("Atmosphere");

				auto& createFolder = createFolderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createScene = createSceneMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

				auto& createEmptyMaterial = createEmptyMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createStandardMaterial = createStandardMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createUnlitMaterial = createUnlitMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createSkysphereMaterial = createSkysphereMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createAtmosphereMaterial = createAtmosphereMaterialMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

				auto& createEmptyShader = createEmptyShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createPartialShader = createPartialShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createStandardShader = createStandardShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createUnlitShader = createUnlitShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createSkysphereShader = createSkysphereShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
				auto& createAtmosphereShader = createAtmosphereShaderMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

				createFolderMenu.ClickedEvent += [&createFolder] { createFolder.content = ""; };
				createSceneMenu.ClickedEvent += [&createScene] { createScene.content = ""; };
				createStandardShaderMenu.ClickedEvent += [&createStandardShader] { createStandardShader.content = ""; };
				createUnlitShaderMenu.ClickedEvent += [&createUnlitShader] { createUnlitShader.content = ""; };
				createSkysphereShaderMenu.ClickedEvent += [&createSkysphereShader] { createSkysphereShader.content = ""; };
				createAtmosphereShaderMenu.ClickedEvent += [&createAtmosphereShader] { createAtmosphereShader.content = ""; };
				createEmptyMaterialMenu.ClickedEvent += [&createEmptyMaterial] { createEmptyMaterial.content = ""; };
				createEmptyShaderMenu.ClickedEvent += [&createEmptyShader] { createEmptyShader.content = ""; };
				createPartialShaderMenu.ClickedEvent += [&createPartialShader] { createPartialShader.content = ""; };
				createStandardMaterialMenu.ClickedEvent += [&createStandardMaterial] { createStandardMaterial.content = ""; };
				createUnlitMaterialMenu.ClickedEvent += [&createUnlitMaterial] { createUnlitMaterial.content = ""; };
				createSkysphereMaterialMenu.ClickedEvent += [&createSkysphereMaterial] { createSkysphereMaterial.content = ""; };
				createAtmosphereMaterialMenu.ClickedEvent += [&createAtmosphereMaterial] { createAtmosphereMaterial.content = ""; };

				createFolder.EnterPressedEvent += [this](std::string newFolderName) {
					const auto finalPath = FindAvailableFilePath(filePath / newFolderName);
					std::filesystem::create_directory(finalPath);
					ItemAddedEvent.Invoke(finalPath);
					Close();
				};

				createScene.EnterPressedEvent += [this](std::string newSceneName) {
					const auto finalPath = FindAvailableFilePath(filePath / (newSceneName + ".ovscene"));

					auto emptyScene = OvCore::SceneSystem::Scene{};
					emptyScene.AddDefaultCamera();
					emptyScene.AddDefaultLights();

					EDITOR_EXEC(SaveSceneToDisk(emptyScene, finalPath.string()));

					ItemAddedEvent.Invoke(finalPath);
					Close();
				};

				createPartialShader.EnterPressedEvent += [this](std::string newShaderName) {
					const auto finalPath = FindAvailableFilePath(filePath / (newShaderName + ".ovfxh"));

					{
						std::ofstream outfile(finalPath);
					}

					ItemAddedEvent.Invoke(finalPath);
					Close();
				};

				CreateNewShaderCallback(createEmptyShader);
				CreateNewShaderCallback(createStandardShader, "Standard");
				CreateNewShaderCallback(createUnlitShader, "Unlit");
				CreateNewShaderCallback(createSkysphereShader, "Skysphere");
				CreateNewShaderCallback(createAtmosphereShader, "Atmosphere");

				CreateNewMaterialCallback(createEmptyMaterial);
				CreateNewMaterialCallback(createStandardMaterial, "Standard");
				CreateNewMaterialCallback(createUnlitMaterial, "Unlit");
				CreateNewMaterialCallback(createSkysphereMaterial, "Skysphere", [](OvCore::Resources::Material& material) {
					// A default skysphere material should have backface culling disabled
					// And frontface culling enabled (renders the inside of the sphere).
					material.SetDrawOrder(100);
					material.SetDepthTest(false);
					material.SetDepthWriting(false);
					material.SetBackfaceCulling(false);
					material.SetFrontfaceCulling(true);
				});
				CreateNewMaterialCallback(createAtmosphereMaterial, "Atmosphere", [](OvCore::Resources::Material& material) {
					// A default atmosphere material should have backface culling disabled
					// And frontface culling enabled (renders the inside of the cube).
					material.SetDrawOrder(10);
					material.SetDepthTest(false);
					material.SetDepthWriting(false);
					material.SetBackfaceCulling(false);
					material.SetFrontfaceCulling(true);
				});

				BrowserItemContextualMenu::CreateList();
			}
		}

		virtual void DeleteItem() override
		{
			using namespace OvWindowing::Dialogs;
			MessageBox message(
				"Delete folder",
				std::format(
					"Deleting a folder (and all its content) is irreversible, are you sure that you want to delete \"{}\"?",
					filePath.string()
				),
				MessageBox::EMessageType::WARNING,
				MessageBox::EButtonLayout::YES_NO
			);

			if (message.GetUserAction() == MessageBox::EUserAction::YES)
			{
				if (std::filesystem::exists(filePath) == true)
				{
					EDITOR_EXEC(PropagateFolderDestruction(filePath.string()));
					std::filesystem::remove_all(filePath);
					DestroyedEvent.Invoke(filePath);
				}
			}
		}

	public:
		OvTools::Eventing::Event<std::filesystem::path> ItemAddedEvent;
	};

	class ScriptFolderContextualMenu : public FolderContextualMenu
	{
	public:
		ScriptFolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FolderContextualMenu(p_filePath, p_protected) {}

		void CreateScript(const std::string& p_name, const std::string& p_path)
		{
			const std::string fileContent = EDITOR_CONTEXT(scriptEngine)->GetDefaultScriptContent(p_name);

			std::ofstream outfile(p_path);
			outfile << fileContent << std::endl;

			ItemAddedEvent.Invoke(p_path);
			Close();
		}

		virtual void CreateList() override
		{
			FolderContextualMenu::CreateList();

			auto& newScriptMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("New script...");
			auto& nameEditor = newScriptMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");

			newScriptMenu.ClickedEvent += [this, &nameEditor] {
				nameEditor.content = OvTools::Utils::PathParser::GetElementName("");
			};

			nameEditor.EnterPressedEvent += [this](std::string p_newName) {
				// Clean the name (Remove special chars)
				std::erase_if(p_newName, [](char c) { 
					return std::find(kAllowedFilenameChars.begin(), kAllowedFilenameChars.end(), c) == kAllowedFilenameChars.end();
				});

				const auto extension = EDITOR_CONTEXT(scriptEngine)->GetDefaultExtension();
				const auto newPath = filePath / (p_newName + extension);

				if (!std::filesystem::exists(newPath))
				{
					CreateScript(p_newName, newPath.string());
				}
			};
		}
	};

	class FileContextualMenu : public BrowserItemContextualMenu
	{
	public:
		FileContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

		virtual void CreateList() override
		{
			auto& editAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Open");

			editAction.ClickedEvent += [this] {
				OvTools::Utils::SystemCalls::OpenFile(filePath.string());
			};

			if (!m_protected)
			{
				auto& duplicateAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Duplicate");

				duplicateAction.ClickedEvent += [this] {
					const auto finalPath = FindAvailableFilePath(filePath);
					std::filesystem::copy(filePath, finalPath);
					DuplicateEvent.Invoke(finalPath);
				};
			}

			BrowserItemContextualMenu::CreateList();

			auto& editMetadata = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Properties");

			editMetadata.ClickedEvent += [this] {
				auto& panel = EDITOR_PANEL(OvEditor::Panels::AssetProperties, "Asset Properties");
				const std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected));
				panel.SetTarget(resourcePath);
				panel.Open();
				panel.Focus();
			};
		}

		virtual void DeleteItem() override
		{
			using namespace OvWindowing::Dialogs;
			MessageBox message(
				"Delete file",
				std::format("Deleting a file is irreversible, are you sure that you want to delete \"{}\"?",
					filePath.string()
				),
				MessageBox::EMessageType::WARNING,
				MessageBox::EButtonLayout::YES_NO
			);

			if (message.GetUserAction() == MessageBox::EUserAction::YES)
			{
				RemoveAsset(filePath.string());
				DestroyedEvent.Invoke(filePath);
				EDITOR_EXEC(PropagateFileRename(filePath.string(), "?"));
			}
		}

	public:
		OvTools::Eventing::Event<std::filesystem::path> DuplicateEvent;
	};

	template<typename Resource, typename ResourceLoader>
	class PreviewableContextualMenu : public FileContextualMenu
	{
	public:
		PreviewableContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

		virtual void CreateList() override
		{
			auto& previewAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Preview");

			previewAction.ClickedEvent += [this] {
				OpenInAssetView(GetResource<ResourceLoader>(filePath.string(), m_protected));
			};

			FileContextualMenu::CreateList();
		}
	};

	class ShaderContextualMenu : public FileContextualMenu
	{
	public:
		ShaderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

		virtual void CreateList() override
		{
			FileContextualMenu::CreateList();

			auto& compileAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Compile");

			compileAction.ClickedEvent += [this] {
				using namespace OvRendering::Resources::Loaders;
				auto& shaderManager = OVSERVICE(OvCore::ResourceManagement::ShaderManager);
				const std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected));
				const auto previousLoggingSettings = ShaderLoader::GetLoggingSettings();
				auto newLoggingSettings = previousLoggingSettings;
				newLoggingSettings.summary = true; // Force enable summary logging
				ShaderLoader::SetLoggingSettings(newLoggingSettings);

				if (shaderManager.IsResourceRegistered(resourcePath))
				{
					// Trying to recompile
					shaderManager.ReloadResource(shaderManager[resourcePath], filePath.string());
				}
				else
				{
					// Trying to compile
					OVSERVICE(OvCore::ResourceManagement::ShaderManager).LoadResource(resourcePath);
				}

				ShaderLoader::SetLoggingSettings(previousLoggingSettings);
			};
		}
	};

	class ShaderPartContextualMenu : public FileContextualMenu
	{
	public:
		ShaderPartContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}
	};

	class ModelContextualMenu : public PreviewableContextualMenu<OvRendering::Resources::Model, OvCore::ResourceManagement::ModelManager>
	{
	public:
		ModelContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

		void CreateMaterialFiles(const std::string_view shaderType)
		{
			auto& modelManager = OVSERVICE(OvCore::ResourceManagement::ModelManager);
			const std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected));

			if (auto model = modelManager.GetResource(resourcePath))
			{
				for (const std::string& materialName : model->GetMaterialNames())
				{
					const auto finalPath = FindAvailableFilePath(filePath / (materialName + ".ovmat"));

					const std::string fileContent = std::format(
						"<root><shader>:Shaders\\{}.ovfx</shader></root>",
						shaderType
					);

					// Create the material file
					{
						std::ofstream outfile(finalPath);
						outfile << fileContent << std::endl;
					}

					DuplicateEvent.Invoke(finalPath);
				}
			}
		}

		void CreateMaterialCreationOption(OvUI::Internal::WidgetContainer& p_root, const std::string_view p_materialName)
		{
			const std::string materialName{ p_materialName };

			p_root.CreateWidget<OvUI::Widgets::Menu::MenuItem>(materialName).ClickedEvent += [this, p_materialName]
			{
				CreateMaterialFiles(p_materialName);
			};
		}

		virtual void CreateList() override
		{
			auto& reloadAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Reload");

			reloadAction.ClickedEvent += [this]
			{
				auto& modelManager = OVSERVICE(OvCore::ResourceManagement::ModelManager);
				std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected));
				if (modelManager.IsResourceRegistered(resourcePath))
				{
					modelManager.AResourceManager::ReloadResource(resourcePath);
				}
			};

			if (!m_protected)
			{
				auto& generateMaterialsMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>(
					"Generate materials..."
				);

				CreateMaterialCreationOption(generateMaterialsMenu, "Standard");
				CreateMaterialCreationOption(generateMaterialsMenu, "Unlit");
			}

			PreviewableContextualMenu::CreateList();
		}
	};

	class TextureContextualMenu : public PreviewableContextualMenu<OvRendering::Resources::Texture, OvCore::ResourceManagement::TextureManager>
	{
	public:
		TextureContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

		virtual void CreateList() override
		{
			auto& reloadAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Reload");

			reloadAction.ClickedEvent += [this]
			{
				auto& textureManager = OVSERVICE(OvCore::ResourceManagement::TextureManager);
				const std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected));
				if (textureManager.IsResourceRegistered(resourcePath))
				{
					/* Trying to recompile */
					textureManager.AResourceManager::ReloadResource(resourcePath);
					EDITOR_PANEL(OvEditor::Panels::MaterialEditor, "Material Editor").Refresh();
				}
			};

			PreviewableContextualMenu::CreateList();
		}
	};

	class SceneContextualMenu : public FileContextualMenu
	{
	public:
		SceneContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

		virtual void CreateList() override
		{
			auto& editAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Edit");

			editAction.ClickedEvent += [this]
			{
				EDITOR_EXEC(LoadSceneFromDisk(EDITOR_EXEC(GetResourcePath(filePath.string()))));
			};

			FileContextualMenu::CreateList();
		}
	};

	class MaterialContextualMenu : public PreviewableContextualMenu<OvCore::Resources::Material, OvCore::ResourceManagement::MaterialManager>
	{
	public:
		MaterialContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

		virtual void CreateList() override
		{
			auto& editAction = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Edit");

			editAction.ClickedEvent += [this]
			{
				auto material = OVSERVICE(OvCore::ResourceManagement::MaterialManager)[EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected))];

				if (material)
				{
					OpenInAssetView(*material);
					EDITOR_EXEC(DelayAction([material]() { OpenInMaterialEditor(*material); }));
				}
			};

			auto& reload = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Reload");
			reload.ClickedEvent += [this]
			{
				auto materialManager = OVSERVICE(OvCore::ResourceManagement::MaterialManager);
				auto resourcePath = EDITOR_EXEC(GetResourcePath(filePath.string(), m_protected));
				OvCore::Resources::Material* material = materialManager[resourcePath];
				if (material)
				{
					materialManager.AResourceManager::ReloadResource(resourcePath);
					EDITOR_PANEL(OvEditor::Panels::MaterialEditor, "Material Editor").Refresh();
				}
			};

			PreviewableContextualMenu::CreateList();
		}
	};

	FileContextualMenu& CreateFileContextualMenu(
		OvUI::Widgets::AWidget& p_root,
		OvTools::Utils::PathParser::EFileType p_fileType,
		const std::string_view p_path,
		const bool p_protected)
	{
		using enum OvTools::Utils::PathParser::EFileType;
		const std::string path{ p_path };

		switch (p_fileType)
		{
			case MODEL: return p_root.AddPlugin<ModelContextualMenu>(path, p_protected);
			case TEXTURE: return p_root.AddPlugin<TextureContextualMenu>(path, p_protected);
			case SHADER: return p_root.AddPlugin<ShaderContextualMenu>(path, p_protected);
			case SHADER_PART: return p_root.AddPlugin<ShaderPartContextualMenu>(path, p_protected);
			case MATERIAL: return p_root.AddPlugin<MaterialContextualMenu>(path, p_protected);
			case SCENE: return p_root.AddPlugin<SceneContextualMenu>(path, p_protected);
			default: return p_root.AddPlugin<FileContextualMenu>(path, p_protected);
		}
	}
}

OvEditor::Panels::AssetBrowser::AssetBrowser
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	using namespace OvWindowing::Dialogs;

	if (std::filesystem::create_directories(EDITOR_CONTEXT(projectAssetsPath)))
	{
		MessageBox message(
			"Assets folder not found",
			"The \"Assets/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
			MessageBox::EMessageType::WARNING,
			MessageBox::EButtonLayout::OK
		);
	}

	if (std::filesystem::create_directories(EDITOR_CONTEXT(projectScriptsPath)))
	{
		MessageBox message(
			"Scripts folder not found",
			"The \"Scripts/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
			MessageBox::EMessageType::WARNING,
			MessageBox::EButtonLayout::OK
		);
	}

	auto& refreshButton = CreateWidget<Buttons::Button>("Rescan assets");
	refreshButton.ClickedEvent += std::bind(&AssetBrowser::Refresh, this);
	refreshButton.lineBreak = false;
	refreshButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };

	auto& importButton = CreateWidget<Buttons::Button>("Import asset");
	importButton.ClickedEvent += EDITOR_BIND(ImportAsset, EDITOR_CONTEXT(projectAssetsPath).string());
	importButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

	m_assetList = &CreateWidget<Layout::Group>();

	Fill();
}

void OvEditor::Panels::AssetBrowser::Fill()
{
	m_assetList->CreateWidget<OvUI::Widgets::Visual::Separator>();
	ConsiderItem(nullptr, std::filesystem::directory_entry(EDITOR_CONTEXT(engineAssetsPath)), true);
	m_assetList->CreateWidget<OvUI::Widgets::Visual::Separator>();
	ConsiderItem(nullptr, std::filesystem::directory_entry(EDITOR_CONTEXT(projectAssetsPath)), false);
	m_assetList->CreateWidget<OvUI::Widgets::Visual::Separator>();
	ConsiderItem(nullptr, std::filesystem::directory_entry(EDITOR_CONTEXT(projectScriptsPath)), false, false, true);
}

void OvEditor::Panels::AssetBrowser::Clear()
{
	m_assetList->RemoveAllWidgets();
}

void OvEditor::Panels::AssetBrowser::Refresh()
{
	Clear();
	Fill();
}

void OvEditor::Panels::AssetBrowser::ParseFolder(Layout::TreeNode& p_root, const std::filesystem::directory_entry& p_directory, bool p_isEngineItem, bool p_scriptFolder)
{
	// Iterates another time to display list files
	for (auto& item : std::filesystem::directory_iterator(p_directory))
	{
		if (item.is_directory())
		{
			ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
		}
	}

	// Iterates another time to display list files
	for (auto& item : std::filesystem::directory_iterator(p_directory))
	{
		if (!item.is_directory())
		{
			ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
		}
	}
}

void OvEditor::Panels::AssetBrowser::ConsiderItem(OvUI::Widgets::Layout::TreeNode* p_root, const std::filesystem::directory_entry& p_entry, bool p_isEngineItem, bool p_autoOpen, bool p_scriptFolder)
{
	const bool isDirectory = p_entry.is_directory();
	const std::string itemname = OvTools::Utils::PathParser::GetElementName(p_entry.path().string());
	const auto fileType = OvTools::Utils::PathParser::GetFileType(itemname);

	// Unknown file, so we skip it
	if (!isDirectory && fileType == OvTools::Utils::PathParser::EFileType::UNKNOWN)
	{
		return;
	}

	std::string path = p_entry.path().string();

	const std::string resourceFormatPath = EDITOR_EXEC(GetResourcePath(path, p_isEngineItem));
	const bool protectedItem = !p_root || p_isEngineItem;

	/* If there is a given treenode (p_root) we attach the new widget to it */
	auto& itemGroup = p_root ? p_root->CreateWidget<Layout::Group>() : m_assetList->CreateWidget<Layout::Group>();

	/* Find the icon to apply to the item */
	const uint32_t iconTextureID = isDirectory ? EDITOR_CONTEXT(editorResources)->GetTexture("Folder")->GetTexture().GetID() : EDITOR_CONTEXT(editorResources)->GetFileIcon(itemname)->GetTexture().GetID();

	itemGroup.CreateWidget<Visual::Image>(iconTextureID, OvMaths::FVector2{ 16, 16 }).lineBreak = false;

	/* If the entry is a directory, the content must be a tree node, otherwise (= is a file), a text will suffice */
	if (isDirectory)
	{
		auto& treeNode = itemGroup.CreateWidget<Layout::TreeNode>(itemname);

		if (p_autoOpen)
		{
			treeNode.Open();
		}

		auto& ddSource = treeNode.AddPlugin<OvUI::Plugins::DDSource<std::pair<std::string, Layout::Group*>>>("Folder", resourceFormatPath, std::make_pair(resourceFormatPath, &itemGroup));
		
		if (!p_root || p_scriptFolder)
		{
			treeNode.RemoveAllPlugins();
		}

		auto& contextMenu = !p_scriptFolder ? treeNode.AddPlugin<FolderContextualMenu>(path, protectedItem && resourceFormatPath != "") : treeNode.AddPlugin<ScriptFolderContextualMenu>(path, protectedItem && resourceFormatPath != "");
		contextMenu.userData = static_cast<void*>(&treeNode);

		contextMenu.ItemAddedEvent += [this, &treeNode, p_isEngineItem, p_scriptFolder] (std::filesystem::path p_path) {
			treeNode.Open();
			treeNode.RemoveAllWidgets();
			ParseFolder(
				treeNode,
				std::filesystem::directory_entry(p_path.parent_path()),
				p_isEngineItem,
				p_scriptFolder
			);
		};

		if (!p_scriptFolder)
		{
			if (!p_isEngineItem) /* Prevent engine item from being DDTarget (Can't Drag and drop to engine folder) */
			{
				treeNode.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("Folder").DataReceivedEvent += [this, &treeNode, path, p_isEngineItem](std::pair<std::string, Layout::Group*> p_data)
				{
					if (!p_data.first.empty())
					{
						const std::filesystem::path folderReceivedPath = EDITOR_EXEC(GetRealPath(p_data.first));
						const std::filesystem::path folderName = folderReceivedPath.filename();
						const std::filesystem::path prevPath = folderReceivedPath;
						const std::filesystem::path correctPath = m_pathUpdate.contains(&treeNode) ? m_pathUpdate.at(&treeNode) : path;
						const std::filesystem::path newPath = correctPath / folderName;

						if (!std::filesystem::exists(newPath))
						{
							const bool isEngineFolder = p_data.first.starts_with(':');

							// Copy dd folder from Engine resources
							if (isEngineFolder)
							{
								std::filesystem::copy(prevPath, newPath, std::filesystem::copy_options::recursive);
							}
							else
							{
								RenameAsset(prevPath.string(), newPath.string());
								EDITOR_EXEC(PropagateFolderRename(prevPath.string(), newPath.string()));
							}

							treeNode.Open();
							treeNode.RemoveAllWidgets();
							ParseFolder(treeNode, std::filesystem::directory_entry(correctPath), p_isEngineItem);

							if (!isEngineFolder)
							{
								p_data.second->Destroy();
							}
						}
						else if (prevPath != newPath)
						{
							using namespace OvWindowing::Dialogs;
							MessageBox errorMessage(
								"Folder already exists",
								"You can't move this folder to this location because the name is already taken",
								MessageBox::EMessageType::ERROR,
								MessageBox::EButtonLayout::OK
							);
						}
					}
				};

				treeNode.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("File").DataReceivedEvent += [this, &treeNode, path, p_isEngineItem](std::pair<std::string, Layout::Group*> p_data)
				{
					if (!p_data.first.empty())
					{
						std::filesystem::path fileReceivedPath = EDITOR_EXEC(GetRealPath(p_data.first));

						const auto fileName = fileReceivedPath.filename();
						const auto prevPath = fileReceivedPath;
						const auto correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
						const auto newPath = correctPath / fileName;

						if (!std::filesystem::exists(newPath))
						{
							bool isEngineFile = p_data.first.at(0) == ':';

							// Copy dd file from Engine resources
							if (isEngineFile)
							{
								std::filesystem::copy_file(prevPath, newPath);
							}
							else
							{
								RenameAsset(prevPath, newPath);
								EDITOR_EXEC(PropagateFileRename(prevPath.string(), newPath.string()));
							}

							treeNode.Open();
							treeNode.RemoveAllWidgets();
							ParseFolder(treeNode, std::filesystem::directory_entry(correctPath), p_isEngineItem);

							if (!isEngineFile)
							{
								p_data.second->Destroy();
							}
						}
						else if (prevPath != newPath)
						{
							using namespace OvWindowing::Dialogs;

							MessageBox errorMessage(
								"File already exists",
								"You can't move this file to this location because the name is already taken",
								MessageBox::EMessageType::ERROR,
								MessageBox::EButtonLayout::OK
							);
						}
					}
				};
			}

			contextMenu.DestroyedEvent += [&itemGroup](const std::filesystem::path& p_deletedPath) { itemGroup.Destroy(); };

			contextMenu.RenamedEvent += [this, &treeNode, &ddSource, p_isEngineItem](
				const std::filesystem::path& p_prev,
				const std::filesystem::path& p_newPath
			) {
				if (!std::filesystem::exists(p_newPath)) // Do not rename a folder if it already exists
				{
					RenameAsset(p_prev, p_newPath);
					EDITOR_EXEC(PropagateFolderRename(p_prev.string(), p_newPath.string()));
					const auto elementName = p_newPath.filename();
					const auto data = std::filesystem::path{ ddSource.data.first }.parent_path() / elementName;
					ddSource.data.first = data.string();
					ddSource.tooltip = data.string();
					treeNode.name = elementName.string();
					treeNode.Open();
					treeNode.RemoveAllWidgets();
					ParseFolder(treeNode, std::filesystem::directory_entry(p_newPath), p_isEngineItem);
					m_pathUpdate[&treeNode] = p_newPath;
				}
				else
				{
					using namespace OvWindowing::Dialogs;

					MessageBox errorMessage(
						"Folder already exists",
						"You can't rename this folder because the given name is already taken",
						MessageBox::EMessageType::ERROR,
						MessageBox::EButtonLayout::OK
					);
				}
			};

			contextMenu.ItemAddedEvent += [this, &treeNode, p_isEngineItem](std::filesystem::path p_path) {
				treeNode.RemoveAllWidgets();
				ParseFolder(
					treeNode,
					std::filesystem::directory_entry(p_path.parent_path()),
					p_isEngineItem
				);
			};

		}
		
		contextMenu.CreateList();

		treeNode.OpenedEvent += [this, &treeNode, path, p_isEngineItem, p_scriptFolder] {
			treeNode.RemoveAllWidgets();
			std::string updatedPath = OvTools::Utils::PathParser::GetContainingFolder(path) + treeNode.name;
			ParseFolder(treeNode, std::filesystem::directory_entry(updatedPath), p_isEngineItem, p_scriptFolder);
		};

		treeNode.ClosedEvent += [this, &treeNode] {
			treeNode.RemoveAllWidgets();
		};
	}
	else
	{
		auto& clickableText = itemGroup.CreateWidget<Texts::TextClickable>(itemname);

		FileContextualMenu& contextMenu = CreateFileContextualMenu(
			clickableText,
			fileType,
			path,
			protectedItem
		);

		contextMenu.CreateList();

		contextMenu.DestroyedEvent += [&itemGroup](std::filesystem::path p_deletedPath) {
			itemGroup.Destroy();

			if (EDITOR_CONTEXT(sceneManager).GetCurrentSceneSourcePath() == p_deletedPath) // Modify current scene source path if the renamed file is the current scene
			{
				EDITOR_CONTEXT(sceneManager).ForgetCurrentSceneSourcePath();
			}
		};

		auto& ddSource = clickableText.AddPlugin<OvUI::Plugins::DDSource<std::pair<std::string, Layout::Group*>>>(
			"File",
			resourceFormatPath,
			std::make_pair(resourceFormatPath, &itemGroup)
		);

		contextMenu.RenamedEvent += [&ddSource, &clickableText, p_scriptFolder](
			std::filesystem::path p_prev,
			std::filesystem::path p_newPath
		) {
			if (p_newPath != p_prev)
			{
				if (!std::filesystem::exists(p_newPath))
				{
					RenameAsset(p_prev, p_newPath);
					const auto elementName = p_newPath.filename();
					ddSource.data.first = (std::filesystem::path{ ddSource.data.first }.parent_path() / elementName).string();
					ddSource.tooltip = ddSource.data.first;

					if (!p_scriptFolder)
					{
						EDITOR_EXEC(PropagateFileRename(p_prev.string(), p_newPath.string()));

						if (EDITOR_CONTEXT(sceneManager).GetCurrentSceneSourcePath() == p_prev) // Modify current scene source path if the renamed file is the current scene
						{
							EDITOR_CONTEXT(sceneManager).StoreCurrentSceneSourcePath(p_newPath.string());
						}
					}
					else
					{
						EDITOR_EXEC(PropagateScriptRename(p_prev.string(), p_newPath.string()));
					}

					clickableText.content = elementName.string();
				}
				else
				{
					using namespace OvWindowing::Dialogs;

					MessageBox errorMessage(
						"File already exists",
						"You can't rename this file because the given name is already taken",
						MessageBox::EMessageType::ERROR,
						MessageBox::EButtonLayout::OK
					);
				}
			}
		};

		contextMenu.DuplicateEvent += [this, &clickableText, p_root, p_isEngineItem] (std::filesystem::path newItem) {
			EDITOR_EXEC(DelayAction(std::bind(&AssetBrowser::ConsiderItem, this, p_root, std::filesystem::directory_entry{ newItem }, p_isEngineItem, false, false), 0));
		};

		if (fileType == OvTools::Utils::PathParser::EFileType::SOUND ||
			fileType == OvTools::Utils::PathParser::EFileType::SCRIPT ||
			fileType == OvTools::Utils::PathParser::EFileType::SHADER ||
			fileType == OvTools::Utils::PathParser::EFileType::SHADER_PART)
		{
			clickableText.DoubleClickedEvent += [path] {
				OvTools::Utils::SystemCalls::OpenFile(path);
			};
		}

		if (fileType == OvTools::Utils::PathParser::EFileType::MODEL)
		{
			clickableText.DoubleClickedEvent += [path, p_isEngineItem] {
				auto& res = GetResource<OvCore::ResourceManagement::ModelManager>(path, p_isEngineItem);
				OpenInAssetView(res);
			};
		}

		if (fileType == OvTools::Utils::PathParser::EFileType::MATERIAL)
		{
			clickableText.DoubleClickedEvent += [path, p_isEngineItem] {
				auto& res = GetResource<OvCore::ResourceManagement::MaterialManager>(path, p_isEngineItem);
				OpenInAssetView(res);
				EDITOR_EXEC(DelayAction([&res]() { OpenInMaterialEditor(res); }));
			};
		}

		if (fileType == OvTools::Utils::PathParser::EFileType::TEXTURE)
		{
			auto& texturePreview = clickableText.AddPlugin<TexturePreview>();
			texturePreview.SetPath(resourceFormatPath);

			clickableText.DoubleClickedEvent += [path, p_isEngineItem] {
				auto& res = GetResource<OvCore::ResourceManagement::TextureManager>(path, p_isEngineItem);
				OpenInAssetView(res);
			};
		}

		if (fileType == OvTools::Utils::PathParser::EFileType::SCENE)
		{
			clickableText.DoubleClickedEvent += [path] {
				EDITOR_EXEC(LoadSceneFromDisk(EDITOR_EXEC(GetResourcePath(path))));
			};
		}
	}
}
