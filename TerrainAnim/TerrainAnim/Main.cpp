#include "pch.h"
#include "Core/Window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	std::unique_ptr<Window> window = std::make_unique<Window>(hInstance, 1280, 720);
	window->InitAppAndShow(false);

	return 0;
}