#include "pch.h"
#include "Application.h"
#include <algorithm>


Application::Application(HWND window, UINT width, UINT height)
	:
	m_hWnd(window),
	m_width(width),
	m_height(height),
	m_appTimer(Timer()),
	m_currentScene(0)
{
}

Application::~Application()
{
}

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
	m_scenes.push_back(new GridScene(m_width*2, m_height*2));

	std::for_each(m_scenes.begin(), m_scenes.end(), [](Scene* scene) {scene->Load(); });
	
	m_appTimer.Reset();
	m_appTimer.Start();
	return true;
}

static bool doOnce = true;
void Application::Run(const InputEvent& input)
{
	m_appTimer.Tick();
	D3D->ClearBackBuffer({ 0.01f, 0.01f, 0.015f, 1 });

	Update(m_appTimer, input);
	Render();
	GUI();

	/*if (doOnce)
	{
		m_currentScene = 1;
		doOnce = false;
	}*/

	D3D->Present();
}

void Application::Shutdown()
{
	std::for_each(m_scenes.begin(), m_scenes.end(), [](Scene* scene) {scene->Unload(); });
}

void Application::Update(float dt, const InputEvent& input)
{
	if (input.KeyboardState.D1)
		D3D->SetWireframe(false);
	if (input.KeyboardState.D2)
		D3D->SetWireframe(true);

	//m_scenes[m_currentScene]->Update(dt, input);
	std::for_each(m_scenes.begin(), m_scenes.end(), [&](Scene* scene) {scene->Update(dt, input); });
}

void Application::Render()
{
	m_scenes[m_currentScene]->Render();
}

void Application::GUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin("Editor");
	ImGui::SetWindowPos({ 0.0f, 0.0f }, ImGuiCond_Always);
	ImGui::SetWindowSize({ 350.0f, static_cast<float>(m_width) }, ImGuiCond_Once);
	
	if (ImGui::CollapsingHeader("Application", nullptr, ImGuiTreeNodeFlags_Bullet))
	{
		ImGui::Text("How to use the Application");
		ImGui::Text("1 - Set view to solid");
		ImGui::Text("2 - Set view to wirerframe");

		if (ImGui::BeginCombo("##SceneSelect", "Select Scene"))
		{
			if (ImGui::Selectable("Test Scene"))
				m_currentScene = 0;

			if (ImGui::Selectable("Grid Scene"))
				m_currentScene = 1;

			ImGui::EndCombo();
		}
		
		if (ImGui::TreeNode("App Stats"))
		{
			auto& cam = m_scenes[m_currentScene]->GetSceneCamera();

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
