#pragma once
#include "Entities/Entity.h"
#include "Graphics/HeightMap.h"
#include "Core/Timer.h"

class Terrain : public Entity
{
	friend class TerrainGenerator;

public:
	Terrain();
	virtual ~Terrain();

	void LoadFile(const std::string& filename, int width, int height);

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

	// Flattens the terrain
	void Flatten();
	// Applies the saved heightmap
	void ApplyHeightmap();

	std::vector<SimpleVertex>& GetVertices() { return m_terrainVertices; }

private:

	void GenerateBuffers();
	void UpdateBuffers();

	std::unique_ptr<HeightMap> m_heightMap;
	std::unique_ptr<Shader>    m_shader;
	std::vector<SimpleVertex>  m_terrainVertices;
	std::vector<UINT>          m_terrainIndices;
	Texture2D                  m_texDirt0;
	Texture2D                  m_texDirt1;
	Texture2D                  m_texStone;
	Texture2D                  m_texGrass;
	Texture2D                  m_texSnow;
	
	int m_vertexCount;
	int m_faceCount;
	int m_indexCount;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	ComPtr<ID3D11Buffer> m_tessFactorsHS;
	ComPtr<ID3D11Buffer> m_gradientBuffer;
	TessellationFactors  m_tessellationFactors;
	TerrainGradients m_gradients;

	bool m_isHeightMapApplied;
};