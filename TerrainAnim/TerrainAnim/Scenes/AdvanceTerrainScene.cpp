#include "pch.h"
#include "AdvanceTerrainScene.h"
#include "Graphics/Direct3D.h"


AdvanceTerrainScene::AdvanceTerrainScene(UINT width, UINT height)
	:
	Scene(width, height)
{
	if (m_wvpBuffer == nullptr)
		D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));
}

void AdvanceTerrainScene::Load()
{
	m_sceneCamera.Position(Vector3(0, 10, -8));
	LOG("Loaded advanced terrain scene")
}

void AdvanceTerrainScene::Update(float dt, const InputEvent& input)
{
}

void AdvanceTerrainScene::Render()
{
}

void AdvanceTerrainScene::GUI()
{
}

void AdvanceTerrainScene::Unload()
{
}
