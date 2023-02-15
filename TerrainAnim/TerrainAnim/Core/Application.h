#pragma once
#include "Mouse.h"
#include "Keyboard.h"
#include "Graphics/Direct3D.h"
#include "Scenes/TestScene.h"

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
	void Update(float dt = 0.0f);
	void Render();

private:
	HWND m_hWnd;
	UINT m_width, m_height;

	std::unique_ptr<TestScene> m_testScene;
};

