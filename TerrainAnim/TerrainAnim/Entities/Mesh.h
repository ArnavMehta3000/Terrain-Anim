#pragma once
#include "Entities/Entity.h"

class Mesh : public Entity
{
	friend class FBX;

public:
	Mesh();
	~Mesh();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

private:
	void GenBuffers();

private:
	std::string               m_name;
	std::unique_ptr<Shader>   m_shader;
	std::vector<SimpleVertex> m_vertices;
	std::vector<int>          m_indices;

	ComPtr<ID3D11Buffer>      m_vertexBuffer;
	ComPtr<ID3D11Buffer>      m_indexBuffer;
};