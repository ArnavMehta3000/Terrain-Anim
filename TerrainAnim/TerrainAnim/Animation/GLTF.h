#pragma once
#include "Entities/Mesh.h"
#include "External/tinygltf/tiny_gltf.h"

class GLTF
{
public:
	GLTF();
	~GLTF();

	bool Load(const char* filename);

	auto GetMeshList() { return m_meshes; }


private:
	void ProcessMesh();
	void ProcessAnimations();
	void ProcessJoints();

private:
	tinygltf::Model m_model;

	std::vector<Mesh*> m_meshes;
	std::vector<Animation> m_animations;
	float m_scaleFactor;
};