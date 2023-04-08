#pragma once
#include "Core/Object.h"
#include "Graphics/Shaders.h"
#include "Graphics/Texture.h"

class Entity : public Object
{
	friend class GLTF;
public:
	Entity();
	~Entity();

	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;

	// Needs to transposed if sending to shader
	const Matrix& GetWorldMatrix() { return m_worldMatrix; }

private:
	void UpdateTransform();

protected:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;
	Matrix  m_worldMatrix;
};