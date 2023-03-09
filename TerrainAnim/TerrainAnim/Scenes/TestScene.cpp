#include "pch.h"
#include "Scenes/TestScene.h"
#include "Graphics/Direct3D.h"

TestScene::TestScene(UINT width, UINT height)
	:
	Scene(width, height)
{}

void TestScene::Load()
{	
	m_sceneCamera.Position(Vector3(0, 0, -8));
	D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));

	m_cube = std::make_unique<CubeEntity>(L"Textures/PFP.JPG");
	LOG("Loaded test scene");
}

void TestScene::Update(float dt, const InputEvent& input)
{
	m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

	WVPBuffer wvp;
	wvp.World      = m_cube->GetWorldMatrix().Transpose();
	wvp.View       = m_sceneCamera.GetView().Transpose();
	wvp.Projection = m_sceneCamera.GetProjection().Transpose();

	D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);

	m_cube->Update(dt, input);
}

void TestScene::Render()
{
	D3D_CONTEXT->DSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());
	m_cube->Render();
}

void TestScene::GUI()
{
	if (ImGui::CollapsingHeader("Test Scene Settings"))
		m_cube->GUI();
}

void TestScene::Unload()
{
	COM_RELEASE(m_wvpBuffer);

	LOG("Unloaded grid scene");
}
