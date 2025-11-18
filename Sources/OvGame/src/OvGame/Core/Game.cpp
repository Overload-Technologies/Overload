/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tracy/Tracy.hpp>

#include <OvDebug/Logger.h>
#include <OvGame/Core/Game.h>
#include <OvUI/Widgets/Texts/Text.h>

#ifdef _DEBUG
#include <OvRendering/Features/FrameInfoRenderFeature.h>
#endif

OvGame::Core::Game::Game(Context & p_context) :
	m_context(p_context),
	m_sceneRenderer(*p_context.driver),
	m_fpsCounter(*p_context.window)
	#ifdef _DEBUG
	,
	m_driverInfo(*m_context.driver, *m_context.window),
	m_frameInfo(*m_context.window)
	#endif
{
	m_context.uiManager->SetCanvas(m_canvas);
	m_canvas.AddPanel(m_fpsCounter);
	#ifdef _DEBUG
	m_canvas.AddPanel(m_driverInfo);
	m_canvas.AddPanel(m_frameInfo);
	m_sceneRenderer.AddFeature<
		OvRendering::Features::FrameInfoRenderFeature,
		OvRendering::Features::EFeatureExecutionPolicy::ALWAYS
	>();
	#endif

	m_context.sceneManager.LoadScene(m_context.projectSettings.Get<std::string>("start_scene"));
	m_context.sceneManager.GetCurrentScene()->Play();
}

OvGame::Core::Game::~Game()
{
	m_context.sceneManager.UnloadCurrentScene();
}

void OvGame::Core::Game::PreUpdate()
{
	ZoneScoped;

	m_context.device->PollEvents();
}

void RenderCurrentScene(
	OvCore::Rendering::SceneRenderer& p_renderer,
	const OvGame::Core::Context& p_context
)
{
	ZoneScoped;

	if (auto currentScene = p_context.sceneManager.GetCurrentScene())
	{
		if (auto camera = currentScene->FindMainCamera())
		{
			auto [windowWidth, windowHeight] = p_context.window->GetSize();

			p_renderer.AddDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>({
				*currentScene,
			});

			OvRendering::Data::FrameDescriptor frameDescriptor;
			frameDescriptor.renderWidth = windowWidth;
			frameDescriptor.renderHeight = windowHeight;
			frameDescriptor.camera = camera->GetCamera();
			frameDescriptor.outputBuffer = *p_context.framebuffer;

			p_renderer.BeginFrame(frameDescriptor);
			p_renderer.DrawFrame();
			p_renderer.EndFrame();
		}
		else
		{
			p_renderer.Clear(true, true, true);
		}
	}
	else
	{
		p_renderer.Clear(true, true, true);
	}
}

void OvGame::Core::Game::Update(float p_deltaTime)
{
	if (auto currentScene = m_context.sceneManager.GetCurrentScene())
	{
		{
			#ifdef _DEBUG
			ZoneScopedN("Physics Update");
			#endif

			if (m_context.physicsEngine->Update(p_deltaTime))
				currentScene->FixedUpdate(p_deltaTime);
		}

		{
			#ifdef _DEBUG
			ZoneScopedN("Scene Update");
			#endif
			currentScene->Update(p_deltaTime);
			currentScene->LateUpdate(p_deltaTime);
		}

		{
			#ifdef _DEBUG
			ZoneScopedN("Audio Update");
			#endif
			m_context.audioEngine->Update();
		}

		RenderCurrentScene(m_sceneRenderer, m_context);

		auto [windowWidth, windowHeight] = m_context.window->GetSize();
		m_context.framebuffer->BlitToBackBuffer(windowWidth, windowHeight);
	}

	m_context.sceneManager.Update();

	if  (m_context.inputManager->IsKeyPressed(OvWindowing::Inputs::EKey::KEY_F12))
		m_showDebugInformation = !m_showDebugInformation;

	if (m_showDebugInformation)
	{
		m_fpsCounter.Update(p_deltaTime);
		#ifdef _DEBUG
		auto& frameInfoRenderFeature = m_sceneRenderer.GetFeature<OvRendering::Features::FrameInfoRenderFeature>();
		auto& frameInfo = frameInfoRenderFeature.GetFrameInfo();
		m_frameInfo.Update(frameInfo);
		#endif
		m_context.uiManager->Render();
	}
}

void OvGame::Core::Game::PostUpdate()
{
	ZoneScoped;

	m_context.window->SwapBuffers();
	m_context.inputManager->ClearEvents();
	m_context.driver->OnFrameCompleted();
}