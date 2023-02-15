#pragma once
#include "Core/Scene.h"

class TestScene : public Scene
{
public:
	TestScene();
	~TestScene();

	virtual void Update(float dt) override;
	virtual void Render() override;
};