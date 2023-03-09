#include "pch.h"
#include "Scenes/GridScene.h"
#include "Graphics/Direct3D.h"

GridScene::GridScene(UINT width, UINT height, const GridDesc& gridDesc)
	:
	Scene(width, height),
	m_wvpBuffer(nullptr),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_gridDesc(gridDesc),
	m_indexCount(0)
{}

void GridScene::Load()
{
	m_sceneCamera.Position(Vector3(0, 2, -8));
	D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));
	
	Shader::InitInfo desc{};
	desc.VertexShaderFile = L"Shaders/Tess_VS.hlsl";
	desc.PixelShaderFile  = L"Shaders/Tess_PS.hlsl";
	desc.HullShaderFile   = L"Shaders/Tess_HS.hlsl";
	desc.DomainShaderFile = L"Shaders/Tess_DS.hlsl";

	m_shader = std::make_unique<Shader>(desc);

	CreateTerrainVB();
	CreateTerrainIB();
}

void GridScene::Update(float dt, const InputEvent& input)
{
	m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);
	D3D_CONTEXT->DSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());

	WVPBuffer wvp;
	wvp.World      = Matrix::Identity;
	wvp.View       = m_sceneCamera.GetView().Transpose();
	wvp.Projection = m_sceneCamera.GetProjection().Transpose();

	D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void GridScene::Render()
{
	m_shader->BindAll();

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}

void GridScene::GUI()
{
	if (ImGui::CollapsingHeader("Grid Scene Settings"))
	{
		if (ImGui::TreeNode("Grid Description"))
		{
			ImGui::Text("Grid Width: %u", m_gridDesc.Width);
			ImGui::Text("Grid Height: %u", m_gridDesc.Height);

			ImGui::TreePop();
		}
		
		int arr[] = { (int)m_gridDesc.Width, (int)m_gridDesc.Height };
		
		ImGui::DragInt2("Size", arr);
		if (ImGui::IsItemEdited())
		{
			m_gridDesc.Width  = (arr[0] < 2) ? 2 : arr[0];
			m_gridDesc.Height = (arr[1] < 2) ? 2 : arr[1];
			
			CreateTerrainVB();
			CreateTerrainIB();
		}
	}
}

void GridScene::Unload()
{
	COM_RELEASE(m_wvpBuffer);
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
}



void GridScene::CreateTerrainVB()
{
	UINT xSize       = m_gridDesc.Width;
	UINT zSize       = m_gridDesc.Height;
	UINT vertexCount = xSize * zSize;

	float halfWidth  = 0.5f * m_gridDesc.Width;
	float halfHeight = 0.5f * m_gridDesc.Height;

	float dx = static_cast<float>(xSize) / (zSize - 1.0f);
	float dz = static_cast<float>(zSize) / (zSize - 1.0f);

	float du = 1.0f / (zSize - 1.0f);
	float dv = 1.0f / (xSize - 1.0f);

	std::vector<SimpleVertex> vertices(vertexCount);
	// Generate vertices
	for (UINT row = 0; row < xSize; row++)
	{
		float z = halfHeight - row * dz;
		for (UINT col = 0; col < zSize; col++)
		{
			float x = -halfWidth + col * dx;
			float y = 0.0f;

			vertices[row * zSize + col] = { Vector3(x, y, z), 
											Vector3(0.0f, 1.0f, 0.0f), 
											Vector2(col * du, row * dv) };
		}
	}

	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage          = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth      = sizeof(SimpleVertex) * vertexCount;
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vsd);
	vsd.pSysMem = vertices.data();

	HR(D3D_DEVICE->CreateBuffer(&vbd, &vsd, m_vertexBuffer.ReleaseAndGetAddressOf()));
}

void GridScene::CreateTerrainIB()
{
	UINT xSize     = m_gridDesc.Width;
	UINT zSize     = m_gridDesc.Height;
	UINT faceCount = (xSize - 1) * (zSize - 1) * 2;

	UINT tris = 0;

	std::vector<UINT> indices(faceCount * 3);  // 3 verts per face
	// Generate indices
	for (UINT row = 0; row < xSize - 1; row++)
	{
		for (UINT col = 0; col < zSize - 1; col++)
		{
			indices[tris]     = row * zSize + col;
			indices[tris + 1] = row * zSize + col + 1;
			indices[tris + 2] = (row + 1) * zSize + col;
			indices[tris + 3] = (row + 1) * zSize + col;
			indices[tris + 4] = row * zSize + col + 1;
			indices[tris + 5] = (row + 1) * zSize + col + 1;
			
			// Next quad
			tris += 6;
		}
	}

	m_indexCount = faceCount * 3;

	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage          = D3D11_USAGE_DYNAMIC;
	ibd.ByteWidth      = sizeof(UINT) * (faceCount * 3);
	ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, isd);
	isd.pSysMem = indices.data();

	HR(D3D_DEVICE->CreateBuffer(&ibd, &isd, m_indexBuffer.ReleaseAndGetAddressOf()));
}
