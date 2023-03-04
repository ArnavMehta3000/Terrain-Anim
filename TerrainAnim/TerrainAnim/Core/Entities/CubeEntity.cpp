#include "pch.h"
#include "CubeEntity.h"
#include "Graphics/Direct3D.h"
#include "Core/Primitives.h"

CubeEntity::CubeEntity()
	:
	Entity(),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_shader(nullptr)
{
	Shader::InitInfo desc{};
	desc.VertexShaderFile = L"Shaders/Default_VS.hlsl";
	desc.PixelShaderFile  = L"Shaders/Default_PS.hlsl";
	desc.HullShaderFile   = L"Shaders/Default_HS.hlsl";
	desc.DomainShaderFile = L"Shaders/Default_DS.hlsl";

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
}

void CubeEntity::Render()
{
	Entity::Render();
	m_shader->BindAll();

	// For tessellation purposes
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

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
}
