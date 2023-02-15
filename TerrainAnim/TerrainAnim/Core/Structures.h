#pragma once

struct InputEvent
{
	DirectX::Keyboard::State KeyboardState;
	DirectX::Mouse::State MouseState;
};

struct SimpleVertex
{
	Vector3 Pos;
	Vector3 Normal;
	Vector2 TexCoord;
};

struct WVPBuffer
{
	Matrix World;
	Matrix View;
	Matrix Projection;
};