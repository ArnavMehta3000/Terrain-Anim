#pragma once
#include "Core/Scene.h"

class AdvancedTerrainScene : public Scene
{
public:
	AdvancedTerrainScene(UINT width, UINT height);
	~AdvancedTerrainScene() final = default;

	void Load() override;
	void Update(float dt, const InputEvent& input);
	void Render() override;
	void GUI() override;
	void Unload() override;

private:
};

