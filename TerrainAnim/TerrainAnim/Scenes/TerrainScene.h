#pragma once
#include "Core/Scene.h"
#include "Entities/Terrain.h"

class TerrainScene : public Scene
{
public:
	TerrainScene(UINT width, UINT height);
	~TerrainScene() final = default;

	void Load() override;
	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;
	void Unload() override;

private:
	std::unique_ptr<Terrain> m_terrain;
};