#include "pch.h"
#include "Scenes/GridScene.h"
#include "Core/Entities/GridEntity.h"
#include "Graphics/Direct3D.h"

GridScene::GridScene(UINT width, UINT height)
	:
	Scene(width, height)
{}

void GridScene::Load()
{
	m_sceneCamera.Position(Vector3(0, 10, -8));
	D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));

	m_grid = std::make_unique<GridEntity>();
	LOG("Loaded grid scene");
}

void GridScene::Update(float dt, const InputEvent& input)
{
	m_grid->Update(dt, input);

	m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

	WVPBuffer wvp;
	wvp.World      = Matrix::Identity;
	wvp.View       = m_sceneCamera.GetView().Transpose();
	wvp.Projection = m_sceneCamera.GetProjection().Transpose();

	D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void GridScene::Render()
{
	D3D_CONTEXT->DSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());
	m_grid->Render();
}

void GridScene::GUI()
{
	if (ImGui::CollapsingHeader("Grid Scene Settings"))
		m_grid->GUI();
}

void GridScene::Unload()
{
	COM_RELEASE(m_wvpBuffer);

	LOG("Unloaded grid scene");
}

