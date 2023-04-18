#pragma once
#include "External/tinygltf/tiny_gltf.h"

struct Mesh;
struct Node;
struct Skin;
struct Primitive;

struct Skin
{
	std::string              Name;
	const Node*              SkeletonRoot = nullptr;
	std::vector<Matrix>      InverseBindMatrices;
	std::vector<const Node*> Joints;
};

struct Node
{
	const int                Index;
	int                      SkinTransformsIndex = -1;
	std::string              Name;
	const Node*              Parent = nullptr;
	std::vector<const Node*> Children;
	const Mesh*              LinkedMesh;
	const Skin*              LinkedSkin;
	Vector3                  Position;
	Quaternion               Rotation;
	Vector3                  Scale;


	explicit Node(int index)
		:
		Index(index)
	{}
};

struct Primitive
{
	const UINT FirstIndex;
	const UINT IndexCount;
	const UINT VertexCount;
	const UINT MaterialId;

	Primitive(UINT firstIndex, UINT indexCount, UINT vertexCount, UINT materialId)
		:
		FirstIndex(firstIndex),
		IndexCount(indexCount),
		VertexCount(vertexCount),
		MaterialId(materialId)
	{}

	bool HasIndices() const noexcept { return IndexCount > 0; }
};

struct Mesh
{
	std::string            Name;
	std::vector<Primitive> Primitives;
	
	bool HasPrimitives() { return !Primitives.empty(); }
};

struct AnimationChannel
{
	enum class AnimPathType { Translation, Rotation, Scale, Weights };

	const AnimPathType PathType;
	const Node*        LinkedNode;
	const UINT         SamplerIndex;

	AnimationChannel(AnimPathType pathType, Node* node, UINT samplerIndex)
		:
		PathType(pathType),
		LinkedNode(node),
		SamplerIndex(samplerIndex)
	{}
};

struct AnimationSampler
{
	enum class InterpolationType { Linear, Step, CubicSpline };

	const InterpolationType Interpolation;
	std::vector<float>      Inputs;
	std::vector<Vector4>    OutputsVec4;

	explicit AnimationSampler(InterpolationType interpolation)
		:
		Interpolation(interpolation)
	{}
};

struct Animation
{
	std::string Name;
	std::vector<AnimationSampler> Samplers;
	std::vector<AnimationChannel> Channels;

	float Start = +(std::numeric_limits<float>::max)();
	float End   = -(std::numeric_limits<float>::max)();
};

struct ModelTransform
{
	struct SkinTransforms
	{
		std::vector<Matrix> JontMatrices;
	};
	struct AnimationTransforms
	{
		Vector3    Position;
		Quaternion Rotation;
		Vector3    Scale = Vector3::One;
	};

	std::vector<Matrix>              NodeLocalMatrices;
	std::vector<Matrix>              NodeGlobalMatrices;
	std::vector<SkinTransforms>      Skins;
	std::vector<AnimationTransforms> NodeAnimations;
};

struct Model
{
	// Uses simple vertex

	struct VertexSkinAtrributes
	{
		Vector4 Joint;
		Vector4 Weight;
	};

	// Node hierarchy
	std::vector<Node*>     RootNodes;
	std::vector<Node>      LinearNodes;
	std::vector<Mesh>      Meshes;
	std::vector<Skin>      Skins;
	std::vector<Animation> Animations;

	// Number of nodes that have skin
	int SkinTransformCount;
};


class GLTF
{
public:
	GLTF();
	~GLTF();

	bool Load(const char* filename);

private:
	void ProcessModel(const tinygltf::Model& model);

private:
	float m_scaleFactor;
};