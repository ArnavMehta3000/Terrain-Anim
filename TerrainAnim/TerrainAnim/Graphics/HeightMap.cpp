#include "pch.h"
#include <fstream>
#include "Graphics/HeightMap.h"


HeightMap::HeightMap(const char* fileName, int w, int h)
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

	for (int i = 0; i < w * h; i++)
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

std::vector<std::vector<float>> HeightMap::GetAs2DArray() const
{
	if (m_heightMap.size() == 0)
	{
		// Should not be requesting heightmap if not initialized		
		assert(false);
	}

	std::vector<std::vector<float>> map;

	// Resize vectors
	map.resize(m_width);
	for (int i = 0; i < m_width; i++)
		map[i].resize(m_height);

	for (int i = 0; i < map.size(); i++)
	{
		int row = i / m_width;
		int col = i % m_height;
		map[row][col] = m_heightMap[i];
	}

	return map;
}
