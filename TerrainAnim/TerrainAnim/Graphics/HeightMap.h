#pragma once

class HeightMap
{
public:
	HeightMap(const char* fileName, int width, int height);
	~HeightMap();
	
	UINT GetWidth() const noexcept { return m_width; }
	UINT GetHeight() const noexcept { return m_height; }

	const std::string& GetFileName() const noexcept { return m_file; }

	float GetValue(UINT i) const noexcept { return m_heightMap[i]; }
	
	std::vector<std::vector<float>> GetAs2DArray() const;

private:
	std::string m_file;
	int m_width;
	int m_height;
	std::vector<float> m_heightMap;
};