#pragma once

struct Shader;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Update(float dt = 0.0f) = 0;
	virtual void Render() = 0;

};