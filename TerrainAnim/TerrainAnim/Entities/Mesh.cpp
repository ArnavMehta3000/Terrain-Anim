#include "pch.h"
#include "Entities/Mesh.h"
#include "Graphics/Direct3D.h"

Mesh::Mesh()
	:
	Entity(),
	m_name(),
	m_vertexBuffer(nullptr),
	m_shader(nullptr)
{
	m_shader = std::make_unique<Shader>(L"Shaders/Anim/Anim_VS.hlsl", L"Shaders/Anim/Anim_PS.hlsl");
}


Mesh::~Mesh()
{
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
	m_name.clear();
}

void Mesh::Update(float dt, const InputEvent& input)
{
	Entity::Update(dt, input);
}

void Mesh::Render()
{
	Entity::Render();
	m_shader->BindVS(true);
	m_shader->BindPS();
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D_CONTEXT->DrawIndexed((UINT)m_indices.size(), 0, 0);
}

void Mesh::GUI()
{
}

void Mesh::GenBuffers()
{
	// Create vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * (UINT)m_vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = m_vertices.data();
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));
	LOG("Created " << m_name << " vertex buffer");


	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(int) * (UINT)m_indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = m_indices.data();
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));
	LOG("Created " << m_name << " index buffer");
}
