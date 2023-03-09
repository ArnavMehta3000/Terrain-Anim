#pragma once
#include "Mouse.h"
#include "Keyboard.h"

constexpr float HS_MAX_TESS_FACTOR = 64.0f;

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

struct TessellationFactors
{
	float EdgeTessFactor = 8.0f;
	float InsideTessFactor = 8.0f;
	Vector2 _pad1;
};