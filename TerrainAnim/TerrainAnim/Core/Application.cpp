#include "pch.h"
#include "Application.h"


Application::Application(HWND window, UINT width, UINT height)
	:
	m_hWnd(window),
	m_width(width),
	m_height(height),
	m_appTimer(Timer())
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

	m_testScene = std::make_unique<TestScene>(m_width, m_height);
	m_testScene->Load();
	
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
	m_testScene->Unload();
}

void Application::Update(float dt, const InputEvent& input)
{
	if (input.KeyboardState.D1)
		D3D->SetWireframe(false);
	if (input.KeyboardState.D2)
		D3D->SetWireframe(true);

	m_testScene->Update(dt, input);
}

void Application::Render()
{
	m_testScene->Render();
}

void Application::GUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin("Editor");
	m_testScene->GUI();
	ImGui::End();
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
