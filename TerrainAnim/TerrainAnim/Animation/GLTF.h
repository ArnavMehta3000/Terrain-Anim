#pragma once
#include "External/tinygltf/tiny_gltf.h"
#include "Animation/Animation.h"

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