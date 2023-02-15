#pragma once
#include "Core/Camera.h"
#include "Core/Structures.h"
struct Shader;

class Scene
{
public:
	Scene(UINT width, UINT height);
	virtual ~Scene();

	virtual void Load() = 0;
	virtual void Update(float dt, const InputEvent& input) = 0;
	virtual void Render() = 0;
	virtual void Unload() = 0;

protected:
	Camera m_sceneCamera;
	UINT m_sceneWidth, m_sceneHeight;
};