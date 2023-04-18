#pragma once
#include <Core/Structures.h>

struct Mesh;
struct Skin;
struct Primitive;
struct Joint;

struct Joint
{
	std::string Name;
};

struct Skin
{
	std::string        Name;
	Matrix             InvBindMatrix;
	std::vector<Joint> Joints;
};

struct Primitive
{
	Mesh* ParentMesh = nullptr;
	Color DiffuseColor;

	std::vector<SimpleVertex> Vertices;
	std::vector<int>          Indices;
};


struct Mesh
{
	std::string            Name;
	std::vector<Primitive> Primitives;
};