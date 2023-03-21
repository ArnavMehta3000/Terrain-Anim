#pragma once
#include "Core/Scene.h"

class DiamondSqrGrid;

class TerrainScene : public Scene
{
public:
	TerrainScene(UINT width, UINT height);
	~TerrainScene() = default;

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

private:
	ComPtr<ID3D11Buffer> m_wvpBuffer;
	std::unique_ptr<DiamondSqrGrid> m_dsTerrain;
};