#pragma once
#include "Core/Scene.h"
#include "Core/Entities/CubeEntity.h"

class TestScene : public Scene
{
public:
	TestScene(UINT width, UINT height);
	~TestScene() {}

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

private:
	ComPtr<ID3D11Buffer>        m_wvpBuffer;
	std::unique_ptr<CubeEntity> m_cube;
};