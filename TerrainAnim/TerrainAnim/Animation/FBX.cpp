#include "pch.h"
#include "FBX.h"
#include <Core/Timer.h>

Vector3& operator<<(Vector3& myVec, const ofbx::Vec3& vec)
{
	myVec.x = static_cast<float>(vec.x);
	myVec.y = static_cast<float>(vec.y);
	myVec.z = static_cast<float>(vec.z);
	return myVec;
}


bool FBX::LoadFBX(const char* filename)
{
	if (m_scene)
		m_scene = nullptr;

	// Check if file exists
	FILE* fp;
	fopen_s(&fp, filename, "rb");
	
	if (fp == NULL) 
		return false;

	// Get filesize 
	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	auto* content = new ofbx::u8[fileSize];
	fread(content, 1, fileSize, fp);

	Timer timer;
	timer.Start();

	// TODO: Set flags before laoding?

	// Ignoring certain nodes will only stop them from being processed not tokenised (i.e. they will still be in the tree)
	ofbx::LoadFlags flags =
		ofbx::LoadFlags::TRIANGULATE         |
		ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
		ofbx::LoadFlags::IGNORE_CAMERAS      |
		ofbx::LoadFlags::IGNORE_LIGHTS       |
		//ofbx::LoadFlags::IGNORE_MODELS     |
		//ofbx::LoadFlags::IGNORE_TEXTURES   |
		//ofbx::LoadFlags::IGNORE_SKIN       |
		//ofbx::LoadFlags::IGNORE_BONES      |
		//ofbx::LoadFlags::IGNORE_PIVOTS     |
		//ofbx::LoadFlags::IGNORE_MATERIALS  |
		//ofbx::LoadFlags::IGNORE_POSES      |
		//ofbx::LoadFlags::IGNORE_ANIMATIONS |
		//ofbx::LoadFlags::IGNORE_LIMBS      |
		//ofbx::LoadFlags::IGNORE_MESHES     |
		ofbx::LoadFlags::IGNORE_VIDEOS;

	m_scene= ofbx::load((ofbx::u8*)content, fileSize, (ofbx::u16)flags);

	timer.Stop();

	if (!m_scene)
	{
		LOG("Failed to load FBX: " << ofbx::getError());
		return false;
	}
	else
	{
		LOG("Loaded fbx in " << timer.TotalTime() * 1000.0f << "ms");
		return true;
	}

	fclose(fp);
}

bool FBX::GenerateMesh()
{ 
	Timer timer;
	const int meshCount = m_scene->getMeshCount();
	for (int i = 0; i < meshCount; i++)
	{
		Mesh myMesh;

		const auto mesh = m_scene->getMesh(i);
		GenerateMeshData(myMesh, mesh);


		m_meshes.push_back(std::make_unique<Mesh>(myMesh));
	}
	timer.Stop();
	LOG("Mesh extraction took " << timer.TotalTime() * 1000.0f << "ms");
	return true;
}

void FBX::GenerateMeshData(Mesh& myMesh, const ofbx::Mesh* mesh)
{
	auto geo = mesh->getGeometry();
	
	auto vertexCount = geo->getVertexCount();
	auto indexCount = geo->getIndexCount();

	auto vertices = geo->getVertices();
	
	myMesh.m_vertices.resize(vertexCount * sizeof(SimpleVertex));
	
	// Get vertex position
	for (int i = 0; i < vertexCount; i++)
	{
		myMesh.m_vertices[i].Pos << vertices[i];
		//LOG("Vertex: " << LOG_VEC(myMesh.m_vertices[i].Pos));
	}

	// Get normals
	for (int i = 0; i < indexCount; i++)
	{

	}
}
