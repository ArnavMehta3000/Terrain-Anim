#include "pch.h"
#include "Application.h"

Application::Application(HWND window, UINT width, UINT height)
	:
	m_hWnd(window),
	m_width(width),
	m_height(height)
{
	D3D;
	D3D;
	D3D;
	D3D;
	D3D;
}

Application::~Application()
{
}

bool Application::Init()
{
	LOG("Use: " << D3D.use_count());
	return true;
}

void Application::Run(const InputEvent input)
{
	LOG("Use: " << D3D.use_count());
}

void Application::Shutdown()
{
}

void Application::Resize()
{
}
