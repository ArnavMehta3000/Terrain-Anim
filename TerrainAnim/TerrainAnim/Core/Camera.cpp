#include "pch.h"
#include "Camera.h"

constexpr auto SPEED_GAIN = 0.00001f;
constexpr auto MAX_SPEED = 1.5f;

Camera::Camera(float angle, float clientWidth, float clientHeight, float nearPlane, float farPlane)
	:
	m_position(Vector3(0.0f, 0.0f, -3.0f)),
	m_rotation(Vector3(0.0f, 0.0f, 0.0f)),
	m_target(Vector3(0.0f, 0.0f, 1.0f)),
	m_up(Vector3::Up),
	m_view(Matrix::Identity),
	m_projection(Matrix::Identity),
	m_viewAngle(XMConvertToRadians(angle)),
	m_width(clientWidth),
	m_height(clientHeight),
	m_nearPlane(nearPlane),
	m_farPlane(farPlane),
	m_speed(0.025f),
	m_sensitivity(0.01f)
{
	// Create the view and projection matrices on creation
	UpdateMatrices();
}


Camera::~Camera()
{
}

void Camera::Update(double dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& mouse)
{
	if (mouse.positionMode == Mouse::MODE_RELATIVE)  // Only update camera when rmb is down
	{
		Move(dt, kb);
		Rotate(dt, mouse);
	}
	UpdateMatrices();
}

void Camera::Move(double dt, const DirectX::Keyboard::State& kb)
{
	// Stores the amount/direction to move this frame
	Vector3 move = Vector3::Zero;
	float height = 0;

	if (kb.W)
		move.z += m_speed;
	if (kb.S)
		move.z -= m_speed;
	if (kb.A)
		move.x -= m_speed;
	if (kb.D)
		move.x += m_speed;

	if (kb.Q)
		height -= m_speed;
	if (kb.E)
		height += m_speed;

	Quaternion q = Quaternion::CreateFromYawPitchRoll(m_rotation);

	move = Vector3::Transform(move, q);
	move *= m_speed;
	m_position += move;
	m_position.y += height * m_speed;
}

void Camera::Rotate(double dt, const DirectX::Mouse::State& mouse)
{
	m_rotation.y += mouse.x * m_sensitivity;
	m_rotation.x += mouse.y * m_sensitivity;

	float change = mouse.scrollWheelValue * SPEED_GAIN;
	m_speed += change;
	m_speed = std::clamp(m_speed, 0.0f, MAX_SPEED);
}

void Camera::UpdateMatrices()
{
	Matrix translation = Matrix::CreateTranslation(m_position);
	Matrix rotation    = Matrix::CreateFromYawPitchRoll(m_rotation);
	Matrix camWorld    = rotation * translation;

	// View matrix is inverse of the world matrix
	m_view = camWorld.Invert();
	m_projection = XMMatrixPerspectiveFovLH(m_viewAngle, m_width / m_height, m_nearPlane, m_farPlane);
}
