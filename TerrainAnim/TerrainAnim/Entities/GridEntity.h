#pragma once
#include "Entities/Entity.h"
#include "Graphics/HeightMap.h"

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

	inline UINT GetGridSize() const noexcept { return static_cast<UINT>(std::pow(2, m_resolution) + 1); }

protected:
	void CreateFlatGrid();

private:
	virtual void CreateTerrainVB();
	virtual void CreateTerrainIB();

protected:
	int m_resolution;

private:
	std::vector<SimpleVertex>  m_vertices;

	std::unique_ptr<Shader>    m_shader;
	Texture2D                  m_texture;
	ComPtr<ID3D11Buffer>       m_vertexBuffer;
	ComPtr<ID3D11Buffer>       m_indexBuffer;
	UINT                       m_indexCount;
							   
	ComPtr<ID3D11Buffer>       m_tessFactorsHS;
	TessellationFactors        m_tessellationFactors;
							   
	float                      m_multiplier;

	std::unique_ptr<HeightMap> m_heightMap;
};