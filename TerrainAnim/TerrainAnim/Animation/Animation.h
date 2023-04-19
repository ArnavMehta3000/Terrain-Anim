#pragma once
#include <Core/Structures.h>

struct Mesh;
struct Primitive;

struct Joint
{
	using JointPtr = std::shared_ptr<Joint>;

	JointPtr            Parent = nullptr;
	int                 Index  = -1;
	std::string         Name;
	Matrix              InverseBindMatrix;
	Matrix              Transform;
	std::vector<JointPtr> Children;

	Joint() = default;
};

struct Skin
{
	std::string     Name;
	Joint::JointPtr JointTree;
};

struct Primitive
{
	Mesh* ParentMesh = nullptr;
	Color DiffuseColor;

	std::vector<SimpleVertex> Vertices;
	std::vector<int>          Indices;
	std::vector<Vector4>      Joints;
	std::vector<Vector4>      Weights;


	UINT                 m_vertexCount  = -1;
	UINT                 m_indexCount   = -1;
	ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_indexBuffer  = nullptr;
};


struct Mesh
{
	std::string            Name;
	Skin                   LinkedSkin;
	std::vector<Primitive> Primitives;
	std::map<Joint*, std::vector<Joint*>> JointMap;
	
	void GeneratePrimitiveBuffers();
};