#pragma once
#include "Entities/Entity.h"

class Mesh : public Entity
{
	friend class GLTF;

public:
	struct Material
	{
		Color Diffuse;
	};

public:
	Mesh();
	~Mesh();
	Mesh(const Mesh& other);

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

	const std::string& GetName() const noexcept { return m_name; }
	const UINT GetVertexCount() const noexcept { return (UINT)m_vertices.size(); }
	const UINT GetIndexCount() const noexcept { return m_indexCount; }


private:
	void GenBuffers();

private:
	std::string               m_name;
	std::unique_ptr<Shader>   m_shader;
	std::vector<SimpleVertex> m_vertices;
	std::vector<int>          m_indices;

	UINT                      m_indexCount;
	Material               m_material;
	int                       m_indexType;
	ComPtr<ID3D11Buffer>      m_materialBuffer;
	ComPtr<ID3D11Buffer>      m_vertexBuffer;
	ComPtr<ID3D11Buffer>      m_indexBuffer;
};