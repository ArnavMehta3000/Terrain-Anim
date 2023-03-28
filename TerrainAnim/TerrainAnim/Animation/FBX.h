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

	void DrawMeshes() const noexcept;

private:
	void GenerateMeshData(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh);
	void GenerateMeshTransform(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh);


private:
	ofbx::IScene*                      m_scene;
	std::vector<std::unique_ptr<Mesh>> m_meshes;
};