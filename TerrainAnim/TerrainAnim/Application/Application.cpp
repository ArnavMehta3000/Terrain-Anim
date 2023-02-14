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
	return true;
}

void Application::Run(const InputEvent input)
{
	LOG(input.MouseState.leftButton)
}

void Application::Shutdown()
{
}

void Application::Resize()
{
}
