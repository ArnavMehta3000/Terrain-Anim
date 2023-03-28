#include "pch.h"
#include "Entities/Mesh.h"

Mesh::Mesh()
	:
	Entity(),
	m_vertexBuffer(nullptr)
{
}

Mesh::~Mesh()
{
	COM_RELEASE(m_vertexBuffer);
}

void Mesh::Update(float dt, const InputEvent& input)
{
	Entity::Update(dt, input);
}

void Mesh::Render()
{
	Entity::Render();
}

void Mesh::GUI()
{
}

void Mesh::GenBuffers()
{
}
