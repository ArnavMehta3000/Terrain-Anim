#include "pch.h"
#include "Core/Entities/Entity.h"

Entity::Entity()
{
	UpdateTransform();
}

Entity::~Entity()
{
}

void Entity::Update(float dt, const InputEvent& input)
{
	UpdateTransform();
}

void Entity::Render()
{
}

void Entity::UpdateTransform()
{
	Matrix scale       = Matrix::CreateScale(m_scale);
	Matrix rotation    = Matrix::CreateFromYawPitchRoll(m_rotation);
	Matrix translation = Matrix::CreateTranslation(m_position);

	m_worldMatrix = scale * rotation * translation;
}
