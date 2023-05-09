#pragma once
#include "Core/Scene.h"

class AdvanceTerrainScene : public Scene
{
public:
	AdvanceTerrainScene(UINT width, UINT height);
	~AdvanceTerrainScene() final = default;

	void Load() override;
	void Update(float dt, const InputEvent& input);
	void Render() override;
	void GUI() override;
	void Unload() override;

private:
};

