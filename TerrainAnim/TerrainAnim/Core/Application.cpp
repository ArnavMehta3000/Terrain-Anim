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

	m_testScene = std::make_unique<TestScene>(m_width, m_height);
	m_testScene->Load();
	
	m_appTimer.Reset();
	m_appTimer.Start();
	return true;
}

void Application::Run(const InputEvent& input)
{
	m_appTimer.Tick();
	Update(m_appTimer, input);
	Render();
}

void Application::Shutdown()
{
	m_testScene->Unload();
}

void Application::Resize()
{
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
	D3D->ClearBackBuffer({ 0.01f, 0.01f, 0.015f, 1 });
	m_testScene->Render();
	D3D->Present();
}
