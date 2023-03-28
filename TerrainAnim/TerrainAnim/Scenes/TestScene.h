#pragma once
#include "Core/Scene.h"
#include "Entities/CubeEntity.h"

class TestScene : public Scene
{
public:
	TestScene(UINT width, UINT height);
	~TestScene() = default;

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

private:
	std::unique_ptr<CubeEntity> m_cube;
};