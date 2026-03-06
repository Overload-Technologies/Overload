/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#define _CRT_SECURE_NO_WARNINGS

#include <filesystem>
#include <fstream>

#include <OvEditor/Core/ProjectHub.h>
#include <OvEditor/Settings/EditorSettings.h>
#include <OvEditor/Utils/FileSystem.h>
#include <OvEditor/Utils/ProjectManagement.h>

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/SystemCalls.h>

#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/Spacing.h>
#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Visual/Separator.h>

#include <OvWindowing/Dialogs/MessageBox.h>
#include <OvWindowing/Dialogs/OpenFileDialog.h>
#include <OvWindowing/Dialogs/SaveFileDialog.h>

namespace OvEditor::Core
{
	class ProjectHubPanel : public OvUI::Panels::PanelWindow
	{
	public:
		ProjectHubPanel() : PanelWindow("Overload - Project Hub", true)
		{
			resizable = false;
			movable = false;
			titleBar = false;

			std::filesystem::create_directories(Utils::FileSystem::kEditorDataPath);

			SetSize({ 1000, 580 });
			SetPosition({ 0.f, 0.f });

			auto& openProjectButton = CreateWidget<OvUI::Widgets::Buttons::Button>("Open Project");
			auto& newProjectButton = CreateWidget<OvUI::Widgets::Buttons::Button>("New Project");
			auto& pathField = CreateWidget<OvUI::Widgets::InputFields::InputText>("");
			m_goButton = &CreateWidget<OvUI::Widgets::Buttons::Button>("GO");

			pathField.ContentChangedEvent += [this, &pathField](std::string p_content) {
				pathField.content = std::filesystem::path{
					p_content
				}.make_preferred().string();

				_UpdateGoButton(pathField.content);
			};

			_UpdateGoButton({});

			openProjectButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
			newProjectButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };

			openProjectButton.ClickedEvent += [this] {
				OvWindowing::Dialogs::OpenFileDialog dialog("Open project");
				dialog.AddFileType("Overload Project", "*.ovproject");
				dialog.Show();

				const std::filesystem::path projectFile = dialog.GetSelectedFilePath();
				const std::filesystem::path projectFolder = projectFile.parent_path();

				if (dialog.HasSucceeded())
				{
					if (!_TryFinish({ projectFolder }))
					{
						_OnFailedToOpenCorruptedProject(projectFolder);
					}
				}
			};

			newProjectButton.ClickedEvent += [this, &pathField] {
				OvWindowing::Dialogs::SaveFileDialog dialog("New project location");
				dialog.DefineExtension("Overload Project", "..");
				dialog.Show();
				if (dialog.HasSucceeded())
				{
					std::string selectedFile = dialog.GetSelectedFilePath();

					if (selectedFile.ends_with(".."))
					{
						selectedFile.erase(selectedFile.size() - 2);
					}

					pathField.content = selectedFile;

					_UpdateGoButton(pathField.content);
				}
			};

			m_goButton->ClickedEvent += [this, &pathField] {
				const std::filesystem::path path = pathField.content;

				if (!Utils::ProjectManagement::CreateProject(path) || !_TryFinish({ path }))
				{
					_OnFailedToCreateProject(path);
				}
			};

			openProjectButton.lineBreak = false;
			newProjectButton.lineBreak = false;
			pathField.lineBreak = false;

			CreateWidget<OvUI::Widgets::Layout::Spacing>();
			CreateWidget<OvUI::Widgets::Visual::Separator>();
			CreateWidget<OvUI::Widgets::Layout::Spacing>();

			auto& columns = CreateWidget<OvUI::Widgets::Layout::Columns<2>>();

			columns.widths = { 750, 500 };

			// Sanitize the project registry before displaying it, so we avoid showing
			// corrupted/deleted projects.
			Utils::ProjectManagement::SanitizeProjectRegistry();
			const auto registeredProjects = Utils::ProjectManagement::GetRegisteredProjects();

			for (const auto& project : registeredProjects)
			{
				auto& text = columns.CreateWidget<OvUI::Widgets::Texts::Text>(project.string());
				auto& actions = columns.CreateWidget<OvUI::Widgets::Layout::Group>();
				auto& openButton = actions.CreateWidget<OvUI::Widgets::Buttons::Button>("Open");
				auto& deleteButton = actions.CreateWidget<OvUI::Widgets::Buttons::Button>("Delete");

				openButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
				deleteButton.idleBackgroundColor = { 0.5f, 0.f, 0.f };

				openButton.ClickedEvent += [this, &text, &actions, project] {
					if (!_TryFinish({ project }))
					{
						text.Destroy();
						actions.Destroy();
						_OnFailedToOpenCorruptedProject(project);
					}
				};

			deleteButton.ClickedEvent += [this, &text, &actions, project] {
				text.Destroy();
				actions.Destroy();
				Utils::ProjectManagement::UnregisterProject(project);
			};

			openButton.lineBreak = false;
			}
		}
		
		std::optional<ProjectHubResult> GetResult() const
		{
			return m_result;
		}

		void Draw() override
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 50.f, 50.f });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

			OvUI::Panels::PanelWindow::Draw();

			ImGui::PopStyleVar(2);
		}

	private:
		void _UpdateGoButton(const std::string& p_path)
		{
			const bool validPath = !p_path.empty();
			m_goButton->idleBackgroundColor = validPath ? OvUI::Types::Color{ 0.f, 0.5f, 0.0f } : OvUI::Types::Color{ 0.1f, 0.1f, 0.1f };
			m_goButton->disabled = !validPath;
		}

		void _OnFailedToOpenCorruptedProject(const std::filesystem::path& p_projectPath)
		{
			Utils::ProjectManagement::UnregisterProject(p_projectPath);

			_ShowError(
				"Invalid project",
				"The selected project is invalid or corrupted.\nPlease select another project."
			);
		}

		void _OnFailedToCreateProject(const std::filesystem::path& p_projectPath)
		{
			_ShowError(
				"Project creation failed",
				"Something went wrong while creating the project.\nPlease ensure the path is valid and you have the necessary permissions."
			);
		}

		void _ShowError(const std::string& p_title, const std::string& p_message)
		{
			using namespace OvWindowing::Dialogs;

			MessageBox errorMessage(
				p_title,
				p_message,
				MessageBox::EMessageType::ERROR,
				MessageBox::EButtonLayout::OK
			);
		}

		bool _ValidateResult(ProjectHubResult& p_result)
		{
			return Utils::ProjectManagement::IsValidProjectDirectory(p_result.projectPath);
		}

		bool _TryFinish(ProjectHubResult p_result)
		{
			if (_ValidateResult(p_result))
			{
				m_result = p_result;
				Close();
				return true;
			}

			return false;
		}

	private:
		std::optional<ProjectHubResult> m_result;
		OvUI::Widgets::Buttons::Button* m_goButton = nullptr;
	};
}

OvEditor::Core::ProjectHub::ProjectHub()
{
	SetupContext();
}

std::optional<OvEditor::Core::ProjectHubResult> OvEditor::Core::ProjectHub::Run()
{
	ProjectHubPanel panel;

	m_uiManager->SetCanvas(m_canvas);
	m_canvas.AddPanel(panel);

	while (!m_window->ShouldClose())
	{
		m_device->PollEvents();
		m_uiManager->Render();
		m_window->SwapBuffers();

		if (!panel.IsOpened())
		{
			m_window->SetShouldClose(true);
		}
	}

	return panel.GetResult();
}

void OvEditor::Core::ProjectHub::SetupContext()
{
	/* Settings */
	OvWindowing::Settings::DeviceSettings deviceSettings;
	OvWindowing::Settings::WindowSettings windowSettings;
	windowSettings.title = "Overload - Project Hub";
	windowSettings.width = 1000;
	windowSettings.height = 580;
	windowSettings.maximized = false;
	windowSettings.resizable = false;
	windowSettings.decorated = true;

	/* Window creation */
	m_device = std::make_unique<OvWindowing::Context::Device>(deviceSettings);
	m_window = std::make_unique<OvWindowing::Window>(*m_device, windowSettings);
	m_window->MakeCurrentContext();

	auto [monWidth, monHeight] = m_device->GetMonitorSize();
	auto [winWidth, winHeight] = m_window->GetSize();
	m_window->SetPosition(monWidth / 2 - winWidth / 2, monHeight / 2 - winHeight / 2);

	/* Graphics context creation */
	m_driver = std::make_unique<OvRendering::Context::Driver>(OvRendering::Settings::DriverSettings{ false });

	m_uiManager = std::make_unique<OvUI::Core::UIManager>(m_window->GetGlfwWindow(),
		static_cast<OvUI::Styling::EStyle>(OvEditor::Settings::EditorSettings::ColorTheme.Get())
	);

	const auto fontPath = std::filesystem::current_path() / "Data" / "Editor" / "Fonts" / "Ruda-Bold.ttf";

	m_uiManager->LoadFont(std::string{ Settings::GetFontID(Settings::EFontSize::BIG) }, fontPath.string(), 20);
	m_uiManager->LoadFont(std::string{ Settings::GetFontID(Settings::EFontSize::MEDIUM) }, fontPath.string(), 18);
	m_uiManager->LoadFont(std::string{ Settings::GetFontID(Settings::EFontSize::SMALL) }, fontPath.string(), 16);
	m_uiManager->UseFont(std::string{ Settings::GetFontID(
		static_cast<Settings::EFontSize>(Settings::EditorSettings::FontSize.Get())
	) });

	m_uiManager->EnableEditorLayoutSave(false);
	m_uiManager->EnableDocking(false);
}
