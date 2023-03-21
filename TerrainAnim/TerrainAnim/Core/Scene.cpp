#include "pch.h"
#include "Core/Scene.h"


Scene::Scene(UINT width, UINT height)
	:
	m_sceneWidth(width),
	m_sceneHeight(height),
	m_sceneCamera(90.0f, static_cast<float>(width), static_cast<float>(height), 0.01f, 10000.0f)
{
	LOG("Scene created");
}

Scene::~Scene()
{
	LOG("Scene Destroyed");
}