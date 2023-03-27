#include "pch.h"
#include "FBX.h"
#include <Core/Timer.h>

bool FBX::LoadFBX(const char* filename)
{
	if (m_fbxScene)
		m_fbxScene = nullptr;

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

	m_fbxScene= ofbx::load((ofbx::u8*)content, fileSize, (ofbx::u16)flags);

	timer.Stop();

	if (!m_fbxScene)
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
