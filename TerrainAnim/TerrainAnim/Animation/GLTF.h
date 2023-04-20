#pragma once
#include "External/tinygltf/tiny_gltf.h"
#include "Animation/Animation.h"

class GLTF
{
public:
	GLTF();
	~GLTF();

	bool Load(const char* filename);
	const auto& GetMeshes() const noexcept { return m_meshes; }
	const auto& GetParentMap() const noexcept { return m_parentMap; }

private:
	void ProcessModel(const tinygltf::Model& model);

private:
	float                           m_scaleFactor;
	std::vector<Mesh*>              m_meshes;
	std::map<int, std::vector<int>> m_parentMap;
};