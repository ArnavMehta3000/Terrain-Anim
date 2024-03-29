#include "pch.h"
#include "CubeEntity.h"
#include "Graphics/Direct3D.h"
#include "Core/Primitives.h"

CubeEntity::CubeEntity(const wchar_t* textureFile)
	:
	Entity(),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_shader(nullptr),
	m_texture(textureFile)
{
	ZeroMemory(&m_tessellationFactors, sizeof(TessellationFactors));
	m_tessellationFactors.EdgeTessFactor = 1.0f;
	m_tessellationFactors.InsideTessFactor = 1.0f;

	D3D->CreateConstantBuffer(m_tessFactorsHS, sizeof(TessellationFactors));

	Shader::InitInfo desc{};
	desc.VertexShaderFile = L"Shaders/Tessellation/Tess_VS.hlsl";
	desc.PixelShaderFile  = L"Shaders/Tessellation/Tess_PS.hlsl";
	desc.HullShaderFile   = L"Shaders/Tessellation/Tess_HS.hlsl";
	desc.DomainShaderFile = L"Shaders/Tessellation/Tess_DS.hlsl";

	m_shader = std::make_unique<Shader>(desc);

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

CubeEntity::~CubeEntity()
{
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
}

void CubeEntity::Update(float dt, const InputEvent& input)
{
	Entity::Update(dt, input);
	D3D_CONTEXT->UpdateSubresource(m_tessFactorsHS.Get(), 0, nullptr, &m_tessellationFactors, 0, 0);

}

void CubeEntity::Render()
{
	Entity::Render();
	m_shader->BindAll();
	m_texture.SetInPS(0);
	// For tessellation purposes
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	
	D3D_CONTEXT->HSSetConstantBuffers(0, 1, m_tessFactorsHS.GetAddressOf());

	UINT stride = Primitives::Cube::VerticesTypeSize;
	UINT offset = 0;
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D_CONTEXT->DrawIndexed(Primitives::Cube::IndicesCount, 0, 0);
}

void CubeEntity::GUI()
{
	if (ImGui::TreeNode("Cube Transform"))
	{
		ImGui::DragFloat3("Cube Position", &m_position.x, 0.1f);
		ImGui::DragFloat3("Cube Rotation", &m_rotation.x, 0.1f);
		ImGui::DragFloat3("Cube Scale", &m_scale.x, 0.1f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Cube Tessellation"))
	{
		ImGui::DragFloat("Edge", &m_tessellationFactors.EdgeTessFactor, 0.1f, 0.01f, HS_MAX_TESS_FACTOR);
		ImGui::DragFloat("Inside", &m_tessellationFactors.InsideTessFactor, 0.1f, 0.01f, HS_MAX_TESS_FACTOR);
		ImGui::TreePop();
	}
}
