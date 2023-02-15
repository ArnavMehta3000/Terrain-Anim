#pragma once
#include "Core/Application.h"


class Window
{
public:
	Window(HINSTANCE hInst, UINT width, UINT height);
	~Window();

	void InitAppAndShow(bool showMaximized = true);

	bool ProcessMessages();

	static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	const UINT GetWindowWidth() const noexcept;
	const UINT GetWindowHeight() const noexcept;

	const UINT GetClientWidth() const noexcept;
	const UINT GetClientHeight() const noexcept;

private:
	HWND                               m_hWnd;
	HINSTANCE                          m_hInst;

	UINT                               m_windowWidth, m_windowHeight;
	UINT                               m_clientWidth, m_clientHeight;

	std::unique_ptr<Application>       m_app;

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse>    m_mouse;

	DirectX::Keyboard::State           m_kbState;
	DirectX::Mouse::State              m_mouseState;
};

