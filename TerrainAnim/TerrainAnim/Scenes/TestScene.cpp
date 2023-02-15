#include "pch.h"
#include "Scenes/TestScene.h"
#include "Graphics/Direct3D.h"
#include "Core/Primitives.h"

TestScene::TestScene(UINT width, UINT height)
	:
	Scene(width, height),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_shader(nullptr)
{
}

void TestScene::Load()
{	
	// Load shader
	m_shader = std::make_unique<Shader>(L"Shaders/Default_VS.hlsl", L"Shaders/Default_PS.hlsl");
	m_shader->BindShader();
	m_sceneCamera.Position(Vector3(0, 0, -8));
	D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());

	// Create vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage          = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth      = Primitives::Cube::VerticesByteWidth;
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = Primitives::Cube::Vertices;
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));
	LOG("Created Cube vertex buffer");

	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage          = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth      = Primitives::Cube::IndicesByteWidth;
	ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = Primitives::Cube::Indices;
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));
	LOG("Created Cube index buffer");
}

void TestScene::Update(float dt, const InputEvent& input)
{
	m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

	WVPBuffer wvp;
	wvp.World      = Matrix::Identity;
	wvp.View       = m_sceneCamera.GetView().Transpose();
	wvp.Projection = m_sceneCamera.GetProjection().Transpose();

	D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void TestScene::Render()
{
	UINT stride = Primitives::Cube::VerticesTypeSize;
	UINT offset = 0;
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D_CONTEXT->DrawIndexed(Primitives::Cube::IndicesCount, 0, 0);
}

void TestScene::Unload()
{
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
}
