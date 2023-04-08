#include "pch.h"
#include "Entities/Mesh.h"
#include "Graphics/Direct3D.h"
#include <External/tinygltf/tiny_gltf.h>

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
	COM_RELEASE(m_indexBuffer);
	m_name.clear();
}

Mesh::Mesh(const Mesh& other)
{
	m_name           = other.m_name;
	m_shader.reset(other.m_shader.get());
	m_vertices       = other.m_vertices;
	m_indices        = other.m_indices;
	m_indexCount     = other.m_indexCount;
	m_material       = other.m_material;
	m_materialBuffer = other.m_materialBuffer;
	m_vertexBuffer   = other.m_vertexBuffer;
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
	D3D_CONTEXT->PSSetConstantBuffers(0, 1, m_materialBuffer.GetAddressOf());

	UINT stride = sizeof(GLTFVertex);
	UINT offset = 0;
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), m_indexType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}

float scaleFactor = 1.0f;
void Mesh::GUI()
{
	if (ImGui::TreeNodeEx(m_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Material: %s", m_material.Name.c_str());
		
		ImGui::Spacing();
		std::string rotName = "Rotation" + m_name;
		std::string scaleName = "Scale" + m_name;
		ImGui::DragFloat3(rotName.c_str(), &m_rotation.x, 0.1f);
		ImGui::DragFloat(scaleName.c_str(), &scaleFactor, 0.1f, 0.001f);
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
	vbd.ByteWidth = sizeof(GLTFVertex) * (UINT)m_vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = m_vertices.data();
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));
	LOG("Created " << m_name << " vertex buffer");

}
