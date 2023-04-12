#pragma once
#include "Entities/Mesh.h"
#include "External/tinygltf/tiny_gltf.h"
#include "Animation/Animation.h"

class GLTF
{
public:
	GLTF();
	~GLTF();

	bool Load(const char* filename);

	auto GetMeshList() { return m_meshes; }


private:
	void ProcessMesh();
	void ProcessSkeleton();
	void GetJointParentMap(std::unordered_map<int, std::vector<int>>& parentMap);
private:
	tinygltf::Model m_model;
	
	std::unordered_map<int, std::vector<int>> m_parentMap;
	std::vector<Mesh*> m_meshes;
	std::vector<Joint> m_joints;
	float m_scaleFactor;
};