#pragma once
#include "Core/Entities/Entity.h"

class GridEntity : public Entity
{
public:
	GridEntity(UINT resolution = 0);
	~GridEntity();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI();

	// Recreate the vertex and index buffers
	void ApplyChanges();

private:
	void CreateTerrainVB();
	void CreateTerrainIB();

private:
	std::unique_ptr<Shader> m_shader;
	ComPtr<ID3D11Buffer>    m_vertexBuffer;
	ComPtr<ID3D11Buffer>    m_indexBuffer;
	UINT                    m_indexCount;

	ComPtr<ID3D11Buffer>    m_tessFactorsHS;
	TessellationFactors     m_tessellationFactors;

	UINT                    m_resolution;
	UINT                    m_gridWidth;
	UINT                    m_gridHeight;
};