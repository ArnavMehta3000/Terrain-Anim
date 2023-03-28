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

	D3D->CreateConstantBuffer(m_materialBuffer, sizeof(Material));
}


Mesh::~Mesh()
{
	COM_RELEASE(m_vertexBuffer);
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

	Material mat
	{
		.Diffuse = m_material.Diffuse
	};

	D3D_CONTEXT->UpdateSubresource(m_materialBuffer.Get(), 0, nullptr, &mat, 0, 0);
	D3D_CONTEXT->PSSetConstantBuffers(0, 0, m_materialBuffer.GetAddressOf());

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->Draw(GetVertexCount(), 0);
}

float scaleFactor = 1.0f;
void Mesh::GUI()
{
	if (ImGui::TreeNodeEx(m_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Scale", &scaleFactor, 0.1f, 0.001f);
		if (ImGui::IsItemEdited())
		{
			m_scale.x = scaleFactor;
			m_scale.y = scaleFactor;
			m_scale.z = scaleFactor;
		}
		ImGui::Text("Vertex Count: %u", GetVertexCount());
		ImGui::Text("Index Count: %u", GetIndexCount());

		ImGui::TreePop();
	}
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

}
