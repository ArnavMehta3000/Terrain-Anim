#pragma once

class HeightMap
{
public:
	HeightMap(const char* fileName, UINT width, UINT height);
	~HeightMap();
	
	UINT GetWidth() const noexcept { return m_width; }
	UINT GetHeight() const noexcept { return m_height; }

	const std::string& GetFileName() const noexcept { return m_file; }

private:
	std::string m_file;
	UINT m_width;
	UINT m_height;
	std::vector<float> m_heightMap;
};