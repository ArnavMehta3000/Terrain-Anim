#pragma once
#include "Core/Scene.h"

class AnimScene : public Scene
{
public:
	AnimScene(UINT width, UINT height);
	~AnimScene() = default;

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

private:
	std::unique_ptr<FBX> m_fbx;
};