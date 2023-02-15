#include "pch.h"
#include "Application.h"

Application::Application(HWND window, UINT width, UINT height)
	:
	m_hWnd(window),
	m_width(width),
	m_height(height)
{
}

Application::~Application()
{
}

bool Application::Init()
{
	if (!D3D->Init(m_hWnd))
		LOG("Failed to initialize Direct3D");

	m_testScene = std::make_unique<TestScene>();

	return true;
}

void Application::Run(const InputEvent input)
{
	Update();
	Render();
}

void Application::Shutdown()
{
}

void Application::Resize()
{
}

void Application::Update(float dt)
{
}

void Application::Render()
{
	D3D->ClearBackBuffer({ 1, 0, 0 ,1 });
	D3D->Present();
}
