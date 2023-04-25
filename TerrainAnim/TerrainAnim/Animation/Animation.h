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

struct SkinnedVertexData
{
	SimpleVertex Vertex;
	Vector4      Joint;
	Vector4      Weight;
};

struct Primitive
{
	using PrimitivePtr = std::shared_ptr<Primitive>;

	Mesh* ParentMesh = nullptr;
	Color DiffuseColor;

	std::vector<SkinnedVertexData> VertexData;
	std::vector<int>               Indices;


	UINT                 m_vertexCount  = -1;
	UINT                 m_indexCount   = -1;
	ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_indexBuffer  = nullptr;

	Primitive() = default;
	~Primitive()
	{
		COM_RELEASE(m_vertexBuffer);
		COM_RELEASE(m_indexBuffer);
		VertexData.clear();
		Indices.clear();
	}
};

struct AnimationChannel 
{
	enum class PathType { Translation, Rotation, Scale, Weights };

	PathType        Path;
	Joint::JointPtr LinkedJoint;
	UINT            SamplerIndex;
};

struct AnimationSampler 
{
	enum class InterpolationType { Linear, Step, CubicSpline };

	InterpolationType    Interpolation;
	std::vector<float>   Input;  // Time
	std::vector<Vector4> Output; // Value
};

struct Animation 
{
	std::string                   Name;
	std::vector<AnimationSampler> Samplers;
	std::vector<AnimationChannel> Channels;
	
	float Start = std::numeric_limits<float>::max();
	float End   = std::numeric_limits<float>::min();
};



struct Mesh
{
	std::string                              Name;
	Skin                                     LinkedSkin;
	std::vector<Primitive::PrimitivePtr>     Primitives;
	std::vector<Animation>                   Animations;
	std::unordered_map<int, Joint::JointPtr> JointIndexMap;

	void GeneratePrimitiveBuffers();
};