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

Vector2& operator<<(Vector2& myVec, const ofbx::Vec2& vec)
{
	myVec.x = static_cast<float>(vec.x);
	myVec.y = static_cast<float>(vec.y);
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

void FBX::DrawMeshes() const noexcept
{
	std::for_each(m_meshes.begin(), m_meshes.end(),
		[](const std::unique_ptr<Mesh>& mesh)
		{
			mesh->Render();
		});
}

bool FBX::GenerateMesh()
{ 
	LOG("\n----- EXTRACTING MESH FROM FBX ------");


	Timer timer;
	const int meshCount = m_scene->getMeshCount();
	for (int i = 0; i < meshCount; i++)
	{
		const auto mesh = m_scene->getMesh(i);
		LOG("\n----- GENERATING MESH: " << mesh->name << " ------");

		std::unique_ptr<Mesh> myMesh = std::make_unique<Mesh>();
		myMesh->m_name = mesh->name;

		GenerateMeshData(myMesh, mesh);
		GenerateMeshTransform(myMesh, mesh);


		myMesh->GenBuffers();
		m_meshes.push_back(std::move(myMesh));
	}
	timer.Stop();

	LOG("\n----- EXTRACTION TIME: " << timer.TotalTime() * 1000.0f << "ms -----");
	return true;
}

void FBX::GenerateMeshData(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh)
{
	auto geo = mesh->getGeometry();

	auto vertexCount = geo->getVertexCount();
	auto indexCount = geo->getIndexCount();

	myMesh->m_vertices.resize(vertexCount * sizeof(SimpleVertex));
	myMesh->m_indices.resize(indexCount * sizeof(int));

	auto indices = geo->getFaceIndices();
	std::vector<int> ind;
	for (int i = 0; i < indexCount; i++)
	{
		ind.push_back((indices[i] < 0) ? -indices[i] : (indices[i] + 1));
		//LOG("Index: " << ind[i]);
	}
	myMesh->m_indicesCount = indexCount;

	// Get vertex position
	auto vertices = geo->getVertices();
	for (int i = 0; i < vertexCount; i++)
	{
		myMesh->m_vertices[i].Pos << vertices[i];
		//LOG("Vertex: " << LOG_VEC(myMesh.m_vertices[i].Pos));
	}

	// Get normals
	bool hasNormals = geo->getNormals() != nullptr;
	if (hasNormals)
	{
		auto normals = geo->getNormals();
		for (int i = 0; i < indexCount; i++)
		{
			myMesh->m_vertices[i].Normal << normals[i];
			//LOG("Normal: " << LOG_VEC(myMesh.m_vertices[i].Normal));
		}
	}

	bool hasUVs = geo->getUVs() != nullptr;
	if (hasUVs)
	{
		auto uv = geo->getUVs();
		for (int i = 0; i < indexCount; i++)
		{
			myMesh->m_vertices[i].TexCoord << uv[i];
			//LOG("Tex Coord: " << myMesh.m_vertices[i].TexCoord.x << ", " << myMesh.m_vertices[i].TexCoord.y);
		}
	}

}

void FBX::GenerateMeshTransform(std::unique_ptr<Mesh>& myMesh, const ofbx::Mesh* mesh)
{
	myMesh->m_position << mesh->getLocalTranslation();
	myMesh->m_rotation << mesh->getLocalRotation();
	myMesh->m_scale << mesh->getLocalScaling();
}
