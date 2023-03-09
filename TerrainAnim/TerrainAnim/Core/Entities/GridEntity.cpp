#include "pch.h"
#include "Core/Entities/GridEntity.h"
#include "Graphics/Direct3D.h"

GridEntity::GridEntity(UINT resolution)
	:
	m_resolution(resolution),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_tessFactorsHS(nullptr),
	m_indexCount(0),
	m_texture(L"Textures/PFP.JPG")
{
	m_gridWidth = 255;
	m_gridHeight = 255;

	ZeroMemory(&m_tessellationFactors, sizeof(TessellationFactors));
	m_tessellationFactors.EdgeTessFactor   = 1.0f;
	m_tessellationFactors.InsideTessFactor = 1.0f;

	Shader::InitInfo desc{};
	desc.VertexShaderFile = L"Shaders/Tess_VS.hlsl";
	desc.PixelShaderFile  = L"Shaders/Tess_PS.hlsl";
	desc.HullShaderFile   = L"Shaders/Tess_HS.hlsl";
	desc.DomainShaderFile = L"Shaders/Tess_DS.hlsl";

	D3D->CreateConstantBuffer(m_tessFactorsHS, sizeof(TessellationFactors));

	m_shader = std::make_unique<Shader>(desc);

	ApplyChanges();
}

GridEntity::~GridEntity()
{
}

void GridEntity::Update(float dt, const InputEvent& input)
{
	D3D_CONTEXT->UpdateSubresource(m_tessFactorsHS.Get(), 0, nullptr, &m_tessellationFactors, 0, 0);
}

void GridEntity::Render()
{
	m_shader->BindAll();
	m_texture.SetInPS();

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	D3D_CONTEXT->HSSetConstantBuffers(0, 1, m_tessFactorsHS.GetAddressOf());
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}

void GridEntity::GUI()
{
	if (ImGui::TreeNode("Grid Settings"))
	{
		ImGui::Text("Grid Resolution: %u", m_resolution);
		ImGui::Text("Grid Index Count: %u", m_indexCount);
		
		ImGui::Spacing();

		ImGui::DragFloat("Edge", &m_tessellationFactors.EdgeTessFactor, 0.1f, 0.1f, HS_MAX_TESS_FACTOR);
		ImGui::DragFloat("Inside", &m_tessellationFactors.InsideTessFactor, 0.1f, 0.1f, HS_MAX_TESS_FACTOR);
		
		ImGui::TreePop();
	}
}


void GridEntity::ApplyChanges()
{
	CreateTerrainVB();
	CreateTerrainIB();
}


void GridEntity::CreateTerrainVB()
{
	UINT xSize       = m_gridWidth;
	UINT zSize       = m_gridHeight;
	UINT vertexCount = xSize * zSize;

	float halfWidth  = 0.5f * m_gridWidth;
	float halfHeight = 0.5f * m_gridHeight;

	float dx = (float)xSize / (zSize - 1.0f);
	float dz = (float)zSize / (zSize - 1.0f);

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

void GridEntity::CreateTerrainIB()
{
	UINT xSize     = m_gridWidth;
	UINT zSize     = m_gridHeight;
	UINT faceCount = (xSize - 1) * (zSize - 1) * 2;

	UINT tris = 0;

	std::vector<UINT> indices(faceCount * 3);  // 3 verts per face
	// Generate indices
	for (UINT row = 0; row < xSize - 1; row++)
	{
		for (UINT col = 0; col < zSize - 1; col++)
		{
			indices[tris] = row * zSize + col;
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
