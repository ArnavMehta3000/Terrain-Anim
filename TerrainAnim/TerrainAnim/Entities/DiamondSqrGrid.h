#pragma once
#include "Entities/GridEntity.h"

class DiamondSqrGrid : public GridEntity
{
public:
	explicit DiamondSqrGrid(UINT resolution = 9);
	virtual ~DiamondSqrGrid();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

	

	void DoDiamondSquareGrid(std::vector<SimpleVertex>& vertices);

private:
	std::unique_ptr<Shader>    m_shader;
	ComPtr<ID3D11Buffer>       m_vertexBuffer;
	ComPtr<ID3D11Buffer>       m_indexBuffer;
	
	UINT                       m_indexCount;
	UINT                       m_vertexCount;

	ComPtr<ID3D11Buffer>       m_tessFactorsHS;
	TessellationFactors        m_tessellationFactors;

	float                      m_heightMultiplier;
};