#pragma once
#include <Core/Structures.h>

struct Mesh;
struct Primitive;

struct Joint
{
	std::string Name;
	Matrix InverseBindMatrix;
	Matrix Transform;
};

struct Skin
{
	std::string        Name;
	Matrix             InvBindMatrix;
};

struct Primitive
{
	Mesh* ParentMesh = nullptr;
	Color DiffuseColor;

	std::vector<SimpleVertex> Vertices;
	std::vector<int>          Indices;
	std::vector<Vector4>      Joints;
	std::vector<Vector4>      Weights;
};


struct Mesh
{
	std::string            Name;
	Skin                   LinkedSkin;
	std::vector<Primitive> Primitives;
};