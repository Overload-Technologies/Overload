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

				UpdateGoButton(pathField.content);
			};

			UpdateGoButton({});

			openProjectButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
			newProjectButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };

			openProjectButton.ClickedEvent += [this] {
				OvWindowing::Dialogs::OpenFileDialog dialog("Open project");
				dialog.AddFileType("Overload Project", "*.ovproject");
				dialog.Show();

				const std::filesystem::path projectPath = dialog.GetSelectedFilePath();

				if (dialog.HasSucceeded())
				{
					OpenProject(projectPath);
				}
			};

			newProjectButton.ClickedEvent += [this, &pathField] {
				OvWindowing::Dialogs::SaveFileDialog dialog("New project location");
				dialog.DefineExtension("Overload Project", "..");
				dialog.Show();
				if (dialog.HasSucceeded())
				{
					std::string result = dialog.GetSelectedFilePath();
					pathField.content = std::string(result.data(), result.data() + result.size() - std::string("..").size()); // remove auto extension
					pathField.content += "\\";
					UpdateGoButton(pathField.content);
				}
			};

			m_goButton->ClickedEvent += [this, &pathField] {
				std::filesystem::path path = pathField.content;

				CreateProject(path);
				OpenProject(path);
			};

			openProjectButton.lineBreak = false;
			newProjectButton.lineBreak = false;
			pathField.lineBreak = false;

			CreateWidget<OvUI::Widgets::Layout::Spacing>(4);
			CreateWidget<OvUI::Widgets::Visual::Separator>();
			CreateWidget<OvUI::Widgets::Layout::Spacing>(4);

			auto& columns = CreateWidget<OvUI::Widgets::Layout::Columns<2>>();

			columns.widths = { 750, 500 };

			std::string line;
			std::ifstream myfile(OvEditor::Utils::FileSystem::kProjectRegistryFilePath);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					if (std::filesystem::exists(line)) // TODO: Delete line from the file
					{
						auto& text = columns.CreateWidget<OvUI::Widgets::Texts::Text>(line);
						auto& actions = columns.CreateWidget<OvUI::Widgets::Layout::Group>();
						auto& openButton = actions.CreateWidget<OvUI::Widgets::Buttons::Button>("Open");
						auto& deleteButton = actions.CreateWidget<OvUI::Widgets::Buttons::Button>("Delete");

						openButton.idleBackgroundColor = { 0.7f, 0.5f, 0.f };
						deleteButton.idleBackgroundColor = { 0.5f, 0.f, 0.f };

						openButton.ClickedEvent += [this, line] {
							OpenProject(line);
						};

						std::string toErase = line;
						deleteButton.ClickedEvent += [this, &text, &actions, toErase] {
							text.Destroy();
							actions.Destroy();

							std::string line;
							std::ifstream fin(Utils::FileSystem::kProjectRegistryFilePath);
							std::ofstream temp("temp");

							while (getline(fin, line))
							{
								if (line != toErase)
								{
									temp << line << std::endl;
								}
							}

							temp.close();
							fin.close();

							std::filesystem::remove(Utils::FileSystem::kProjectRegistryFilePath);
							std::filesystem::rename("temp", Utils::FileSystem::kProjectRegistryFilePath);
						};

						openButton.lineBreak = false;
						deleteButton.lineBreak;
					}
				}
				myfile.close();
			}
		}

		std::optional<ProjectHubResult> GetResult() const { return m_result; }

		void UpdateGoButton(const std::string& p_path)
		{
			const bool validPath = !p_path.empty();
			m_goButton->idleBackgroundColor = validPath ? OvUI::Types::Color{ 0.f, 0.5f, 0.0f } : OvUI::Types::Color{ 0.1f, 0.1f, 0.1f };
			m_goButton->disabled = !validPath;
		}

		void CreateProject(const std::filesystem::path& p_path)
		{
			if (!std::filesystem::exists(p_path))
			{
				std::filesystem::create_directory(p_path);
				std::filesystem::create_directory(p_path / "Assets");
				std::filesystem::create_directory(p_path / "Scripts");
				const std::string directoryName = p_path.stem().string();

				(void)std::ofstream{ 
					p_path / std::format("{}.ovproject", directoryName)
				};
			}
		}

		void OpenProject(const std::filesystem::path& p_path)
		{
			if (!std::filesystem::exists(p_path))
			{
				using namespace OvWindowing::Dialogs;

				MessageBox errorMessage(
					"Project not found",
					"The selected project does not exists",
					MessageBox::EMessageType::ERROR,
					MessageBox::EButtonLayout::OK
				);
			}
			else
			{
				m_result = {
					.projectPath = p_path,
				};

				Close();
			}
		}

		void Draw() override
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 50.f, 50.f });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

			OvUI::Panels::PanelWindow::Draw();

			ImGui::PopStyleVar(2);
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
	m_uiManager->LoadFont("Ruda_Big", "Data\\Editor\\Fonts\\Ruda-Bold.ttf", 18);
	m_uiManager->UseFont("Ruda_Big");
	m_uiManager->EnableEditorLayoutSave(false);
	m_uiManager->EnableDocking(false);
}
