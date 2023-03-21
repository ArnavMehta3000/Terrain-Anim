#pragma once
#include "Core/Scene.h"
#include "Graphics/HeightMap.h"

class GridEntity;

class GridScene : public Scene
{
public:
	GridScene(UINT width, UINT height);
	~GridScene() = default;

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

private:
	HeightMap                   m_heightmap;
	ComPtr<ID3D11Buffer>        m_wvpBuffer;
	std::unique_ptr<GridEntity> m_grid;
};