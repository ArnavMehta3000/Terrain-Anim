#pragma once
#include "Entities/Entity.h"

class Mesh : public Entity
{
	friend class FBX;

public:
	struct FBXMaterial
	{
		std::string Name;
		Color Diffuse;
	};


public:
	Mesh();
	~Mesh();

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
	UINT                      m_indexCount;
	FBXMaterial m_material;

	ComPtr<ID3D11Buffer>      m_materialBuffer;
	ComPtr<ID3D11Buffer>      m_vertexBuffer;

public:
	bool m_Enabled;
};