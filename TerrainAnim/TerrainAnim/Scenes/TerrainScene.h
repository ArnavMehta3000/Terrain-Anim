#pragma once
#include "Core/Scene.h"
#include "Entities/GridEntity.h"

class TerrainScene : public Scene
{
public:
	TerrainScene(UINT width, UINT height);
	virtual ~TerrainScene() = default;

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

private:
	ComPtr<ID3D11Buffer>        m_wvpBuffer;
	std::unique_ptr<GridEntity> m_terrain;
};