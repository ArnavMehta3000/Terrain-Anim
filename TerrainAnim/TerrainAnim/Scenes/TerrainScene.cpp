#include "pch.h"
#include "Scenes/TerrainScene.h"
#include "Graphics/Direct3D.h"

TerrainScene::TerrainScene(UINT width, UINT height)
	:
	Scene(width, height)
{
	if (m_wvpBuffer == nullptr)
		D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));
}

void TerrainScene::Load()
{
	m_sceneCamera.Position(Vector3(0, 10, -8));
	LOG("Loaded terrain scene")

	m_betterTerrain = std::make_unique<Terrain>();
	m_betterTerrain->LoadFile("Assets/Textures513/coastMountain513.raw", 513, 513);
}

void TerrainScene::Update(float dt, const InputEvent& input)
{
	m_betterTerrain->Update(dt, input);
	m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

	WVPBuffer wvp
	{
		.World      = Matrix::Identity,
		.View       = m_sceneCamera.GetView().Transpose(),
		.Projection = m_sceneCamera.GetProjection().Transpose()
	};

	D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void TerrainScene::Render()
{
	D3D_CONTEXT->DSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());
	m_betterTerrain->Render();
}

void TerrainScene::GUI()
{
	if (ImGui::CollapsingHeader("Terrain Scene Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		m_betterTerrain->GUI();
	}
}

void TerrainScene::Unload()
{
	COM_RELEASE(m_wvpBuffer);
	LOG("Unloaded terrain scene")
}
