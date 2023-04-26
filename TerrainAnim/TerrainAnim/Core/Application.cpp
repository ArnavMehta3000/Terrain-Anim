#include "pch.h"
#include "Application.h"
#include "Scenes/TestScene.h"
#include "Scenes/GridScene.h"
#include "Scenes/TerrainScene.h"
#include "Scenes/AnimScene.h"
#include <algorithm>
#include <execution>


Application::Application(HWND window, UINT width, UINT height)
	:
	m_hWnd(window),
	m_width(width),
	m_height(height),
	m_appTimer(Timer()),
	m_currentScene(2)  // TODO: Starts on anim scene
{
	// For WIC Texture loader
	HR(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
}

Application::~Application() {}

bool Application::Init()
{
	if (!D3D->Init(m_hWnd))
		LOG("Failed to initialize Direct3D");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(D3D_DEVICE, D3D_CONTEXT);
	ImGui::StyleColorsDark();

	m_scenes.push_back(new TestScene(m_width, m_height));
	m_scenes.push_back(new GridScene(m_width, m_height));
	m_scenes.push_back(new TerrainScene(m_width, m_height));
	m_scenes.push_back(new AnimScene(m_width, m_height));
	
	// Load all scenes sequentially
	std::for_each(std::execution::seq, m_scenes.begin(), m_scenes.end(), 
		[](Scene* scene)
		{
			scene->Load();
		});

	
	m_appTimer.Reset();
	m_appTimer.Start();
	return true;
}

void Application::Run(const InputEvent& input)
{
	m_appTimer.Tick();
	D3D->ClearBackBuffer({ 0.01f, 0.01f, 0.015f, 1 });

	Update(m_appTimer, input);
	Render();
	GUI();

	D3D->Present();
}

void Application::Shutdown()
{
	std::for_each(m_scenes.begin(), m_scenes.end(), [](Scene* scene) {scene->Unload(); });
}

void Application::Update(float dt, const InputEvent& input)
{
	m_scenes[m_currentScene]->Update(dt, input);
}

void Application::Render()
{
	m_scenes[m_currentScene]->Render();
}

static bool isWireframe = false;

void Application::GUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin("Editor");
	ImGui::SetWindowPos({ 0.0f, 0.0f }, ImGuiCond_Always);
	ImGui::SetWindowSize({ 350.0f, static_cast<float>(m_width) }, ImGuiCond_Once);
	
	if (ImGui::CollapsingHeader("Application", nullptr, ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("How to use the Application");
		ImGui::Text("1 - Set view to solid");
		ImGui::Text("2 - Set view to wirerframe");

		if (ImGui::Button("Toggle Wireframe"))
		{
			isWireframe = !isWireframe;
			D3D->SetWireframe(isWireframe);
		}
		ImGui::SameLine();

		if (ImGui::BeginCombo("##SceneSelect", "Select Scene"))
		{
			if (ImGui::Selectable("Test Scene"))
				m_currentScene = 0;

			if (ImGui::Selectable("Grid Scene"))
				m_currentScene = 1;

			if (ImGui::Selectable("Terrain Scene"))
				m_currentScene = 2;

			if (ImGui::Selectable("Animation Scene"))
				m_currentScene = 3;

			ImGui::EndCombo();
		}
		ImGui::Separator();
		
		if (ImGui::TreeNodeEx("App Stats", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const Camera& cam = m_scenes[m_currentScene]->GetSceneCamera();

			ImGui::Text("Frame Time: %.4fms", m_appTimer.DeltaTime() * 1000.0f);
			ImGui::Text("FPS: %.4f", 1.0f / m_appTimer.DeltaTime());
			ImGui::Text("Camera Pos: (%.3f, %.3f, %.3f)", cam.Position().x, cam.Position().y, cam.Position().z);
			ImGui::Text("Camera Speed: %.3f", cam.GetSpeed());

			ImGui::TreePop();
		}
	}
	
	m_scenes[m_currentScene]->GUI();
	
	ImGui::End();
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
