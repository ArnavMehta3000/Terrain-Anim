#pragma once
#include "Core/Entities/Entity.h"
#include "Graphics/HeightMap.h"

class GridEntity : public Entity
{
public:
	GridEntity(HeightMap map);
	~GridEntity();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI();

	// Recreate the vertex and index buffers
	void ApplyChanges();

protected:
	virtual void CreateTerrainVB();
	virtual void CreateTerrainIB();

private:
	std::unique_ptr<Shader>    m_shader;
	Texture2D                  m_texture;
	ComPtr<ID3D11Buffer>       m_vertexBuffer;
	ComPtr<ID3D11Buffer>       m_indexBuffer;
	UINT                       m_indexCount;
							   
	ComPtr<ID3D11Buffer>       m_tessFactorsHS;
	TessellationFactors        m_tessellationFactors;
							   
	UINT                       m_gridWidth;
	UINT                       m_gridHeight;
	float                      m_multiplier;

	std::unique_ptr<HeightMap> m_heightMap;
};