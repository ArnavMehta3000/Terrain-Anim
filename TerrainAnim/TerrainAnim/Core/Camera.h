#pragma once
#include "Keyboard.h"
#include "Mouse.h"

class Camera
{
public:
	Camera() = default;
	Camera(float angle, float clientWidth, float clientHeight, float nearPlane = 0.01f, float farPlane = 100.0f);
	~Camera();

	void Update(double dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& mouse);

	void Position(Vector3 pos) { m_position = pos; }
	const Vector3& Position() { return m_position; }

	const Matrix& GetView()           const { return m_view; }
	const Matrix& GetProjection()     const { return m_projection; }
	const Matrix  GetViewProjection() const { return m_view * m_projection; }

private:
	void Move(double dt, const DirectX::Keyboard::State& kb);
	void Rotate(double dt, const DirectX::Mouse::State& mouse);
	void UpdateMatrices();

private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_target;
	Vector3 m_up;

	float m_viewAngle;
	float m_width;
	float m_height;
	float m_nearPlane;
	float m_farPlane;

	float m_speed;
	float m_sensitivity;

	Matrix m_view;
	Matrix m_projection;
};

