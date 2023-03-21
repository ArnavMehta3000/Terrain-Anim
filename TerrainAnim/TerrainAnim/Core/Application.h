#pragma once
#include "Mouse.h"
#include "Keyboard.h"
#include "Graphics/Direct3D.h"
#include "Core/Scene.h"
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

private:
	void Update(float dt, const InputEvent& input);
	void Render();
	void GUI();

private:
	HWND                m_hWnd;
	UINT                m_width, m_height;
	Timer               m_appTimer;
	std::vector<Scene*> m_scenes;
	int                 m_currentScene;
};

