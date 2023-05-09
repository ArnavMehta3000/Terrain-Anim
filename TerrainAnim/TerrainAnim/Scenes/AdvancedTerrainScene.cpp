#include "pch.h"
#include "AdvancedTerrainScene.h"
#include "Graphics/Direct3D.h"


AdvancedTerrainScene::AdvancedTerrainScene(UINT width, UINT height)
	:
	Scene(width, height)
{
	if (m_wvpBuffer == nullptr)
		D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));
}

void AdvancedTerrainScene::Load()
{
	m_sceneCamera.Position(Vector3(0, 10, -8));
	LOG("Loaded advanced terrain scene")
}

void AdvancedTerrainScene::Update(float dt, const InputEvent& input)
{
}

void AdvancedTerrainScene::Render()
{
}

void AdvancedTerrainScene::GUI()
{
}

void AdvancedTerrainScene::Unload()
{
}
