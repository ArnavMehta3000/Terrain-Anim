#pragma once
#include "Mouse.h"
#include "Keyboard.h"

constexpr float HS_MAX_TESS_FACTOR = 64.0f;
constexpr int MAX_JOINTS = 128;

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

struct VSAnimInput
{
	Vector3 Pos;
	Vector3 Normal;
	Vector2 TexCoord;
	Vector4 JointWeights;
	Vector4 JointIndices;
};

struct AnimData
{
	Matrix JointMatrices[MAX_JOINTS];
};

struct WVPBuffer
{
	Matrix World;
	Matrix View;
	Matrix Projection;
};

struct TerrainGradients
{
	float Dirt0Height = 10.0f;
	float Dirt1Height = 25.0f;
	float StoneHeight = 30.0f;
	float GrassHeight = 60.0f;
	float SnowHeight  = 100.0f;
	Vector3 _padding;
};

struct TerrainData
{
	bool  UseHeightMap     = false;
	float NoiseWidth       = 513.0f;
	float NoiseHeight      = 513.0f;
	float HeightMultiplier = 100.0f;
};

struct TessellationFactors
{
	float EdgeTessFactor = 8.0f;
	float InsideTessFactor = 8.0f;
	Vector2 _pad1;
};

struct Material
{
	Color Diffuse;
};