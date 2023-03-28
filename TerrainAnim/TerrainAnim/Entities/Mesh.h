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
	std::vector<SimpleVertex> m_vertices;
	ComPtr<ID3D11Buffer> m_vertexBuffer;
};