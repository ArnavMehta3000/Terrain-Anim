#pragma once
#include "External/openfbx/ofbx.h"

class FBX
{
public:
	FBX()  = default;
	~FBX() = default;

	bool LoadFBX(const char* filename);

	const ofbx::IScene* GetScene() const noexcept { return m_fbxScene; }

private:
	ofbx::IScene* m_fbxScene;
};