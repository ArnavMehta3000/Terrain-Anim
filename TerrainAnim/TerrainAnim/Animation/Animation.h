#pragma once
#include <Core/Structures.h>

struct Mesh;
struct Primitive;



struct Primitive
{
	Mesh* ParentMesh;
	Color DiffuseColor;

	std::vector<SimpleVertex> Vertices;
	std::vector<int>          Indices;
};


struct Mesh
{
	std::string            Name;
	std::vector<Primitive> Primitives;
};