#pragma once
#include "Entities/Entity.h"
#include "Graphics/HeightMap.h"
#include "Core/Timer.h"

class GridEntity : public Entity
{
public:
	GridEntity(int resolution = 9);
	virtual ~GridEntity();

	void SetHeightMap(HeightMap map) noexcept;
	void ClearHeightMap() noexcept;

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

	// Recreate the vertex and index buffers
	void ApplyChanges();

	inline int GetGridSize() const noexcept { return (int)std::pow(2, m_resolution) + 1; }

protected:
	void CreateFlatGrid();
	// Alters the y value of the position of the vertices as a 2D array
	void SetHeight(UINT i, UINT j, float height);
	// Alters the y value of the position of the vertices using the index
	void SetHeight(UINT index, float height);

	inline float GetHeight(UINT index) { return m_vertices[index].Pos.y; }
	inline float GetHeight(UINT i, UINT j) { return m_vertices[i * GetGridSize() + j].Pos.y; }

	// Abstract function to be overriden for terrain generation algoeithms
	virtual void Generate() {}

private:
	virtual void CreateTerrainVB();
	virtual void CreateTerrainIB();

protected:
	int m_resolution;
	std::vector<SimpleVertex>  m_vertices;
	float                      m_multiplier;

private:

	std::unique_ptr<Shader>    m_shader;
	Texture2D                  m_texture;
	ComPtr<ID3D11Buffer>       m_vertexBuffer;
	ComPtr<ID3D11Buffer>       m_indexBuffer;
	UINT                       m_indexCount;
							   
	ComPtr<ID3D11Buffer>       m_tessFactorsHS;
	TessellationFactors        m_tessellationFactors;
							   
	std::unique_ptr<HeightMap> m_heightMap;
};