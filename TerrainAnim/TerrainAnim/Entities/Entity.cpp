#include "pch.h"
#include "Entities/Entity.h"

Entity::Entity()
	:
	m_position(Vector3::Zero),
	m_rotation(Vector3::Zero),
	m_scale(Vector3::One)
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
