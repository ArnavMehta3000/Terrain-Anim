#include "pch.h"
#include "TerrainScene.h"
#include "Graphics/Direct3D.h"



TerrainScene::TerrainScene(UINT width, UINT height)
	:
	Scene(width, height)
{}

void TerrainScene::Load()
{
	m_sceneCamera.Position(Vector3(0, 10, -8));
	D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));

	m_dsTerrain = std::make_unique<DiamondSqrGrid>();

	LOG("Loaded terrain scene");
}

void TerrainScene::Update(float dt, const InputEvent& input)
{
	m_dsTerrain->Update(dt, input);

	m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

	WVPBuffer wvp;
	wvp.World = Matrix::Identity;
	wvp.View = m_sceneCamera.GetView().Transpose();
	wvp.Projection = m_sceneCamera.GetProjection();

	D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void TerrainScene::Render()
{
	D3D_CONTEXT->DSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());
	m_dsTerrain->Render();
}

void TerrainScene::GUI()
{
	if (ImGui::CollapsingHeader("Terrain Scene Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		m_dsTerrain->GUI();
	}
}

void TerrainScene::Unload()
{
	COM_RELEASE(m_wvpBuffer);
	LOG("Unloaded gridScene")
}
