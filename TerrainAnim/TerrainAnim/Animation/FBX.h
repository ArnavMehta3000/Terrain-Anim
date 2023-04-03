#pragma once
#include "External/openfbx/ofbx.h"
#include "Entities/Mesh.h"

class FBX
{
public:
	FBX()  = default;
	~FBX() = default;

	bool LoadFBX(const char* filename);
	bool GenerateMesh();

	const ofbx::IScene* GetScene() const noexcept { return m_scene; }

	const std::vector<std::unique_ptr<Mesh>>& GetMeshList() const noexcept { return m_meshes; };

private:
	void ExtractMeshData(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh);
	void ExtractMeshTransform(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh);
	void ExtractMeshMaterials(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh);

	void ExtractBoneData();
	

private:
	ofbx::IScene*                      m_scene;
	std::vector<std::unique_ptr<Mesh>> m_meshes;
};