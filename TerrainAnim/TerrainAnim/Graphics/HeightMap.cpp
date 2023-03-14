#include "pch.h"
#include <fstream>
#include "Graphics/HeightMap.h"


HeightMap::HeightMap(const char* fileName, UINT w, UINT h)
	:
	m_file(fileName),
	m_width(w),
	m_height(h)
{
	std::vector<UCHAR> in(w * h);

	std::ifstream inFile;
	inFile.open(fileName, std::ios_base::binary);

	// Catch file open error here
	if (!inFile)
	{
		LOG("Failed to open heightmap file: " << fileName);
		__debugbreak();
	}

	inFile.read((char*)&in[0], (std::streamsize)in.size());

	m_heightMap.resize((size_t)w * (size_t)h);

	for (UINT i = 0; i < w * h; i++)
	{
		// Cnnvert 0 - 255 to 0.0 - 1.0 range
		m_heightMap[i] = (in[i] / 255.0f);
	}

	LOG("Loaded heightmap file: " << fileName);
}


HeightMap::~HeightMap()
{
	m_heightMap.clear();
}
