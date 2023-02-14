#pragma once
#include "Mouse.h"
#include "Keyboard.h"

struct InputEvent
{
	DirectX::Keyboard::State KeyboardState;
	DirectX::Mouse::State MouseState;
};
class Application
{
public:
	Application(HWND window, UINT width, UINT height);
	~Application();

	bool Init();
	void Run(const InputEvent input);
	void Shutdown();

	void Resize();

private:
	HWND m_hWnd;
	UINT m_width, m_height;
};

