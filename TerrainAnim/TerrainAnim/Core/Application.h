#pragma once
#include "Mouse.h"
#include "Keyboard.h"
#include "Graphics/Direct3D.h"
#include "Scenes/TestScene.h"
#include "Core/Structures.h"
#include "Core/Timer.h"


class Application
{
public:
	Application(HWND window, UINT width, UINT height);
	~Application();

	bool Init();
	void Run(const InputEvent& input);
	void Shutdown();

	void Resize();

private:
	void Update(float dt, const InputEvent& input);
	void Render();

private:
	HWND m_hWnd;
	UINT m_width, m_height;
	Timer m_appTimer;
	std::unique_ptr<TestScene> m_testScene;
};

