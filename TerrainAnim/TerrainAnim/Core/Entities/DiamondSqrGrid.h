#pragma once
#include "Core/Entities/Entity.h"

class DiamondSqrGrid : public Entity
{
public:
	explicit DiamondSqrGrid(UINT resolution = 9);
	virtual ~DiamondSqrGrid();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

	inline float GetGridSize() const noexcept { return static_cast<float>(std::pow(2, m_resolution) + 1); }

private:
	// Create a flat grid of vertices
	std::vector<SimpleVertex> CreateFlatGrid() const;
	void DoDiamondSquareGrid(std::vector<SimpleVertex>& vertices);

private:
	std::unique_ptr<Shader>    m_shader;
	ComPtr<ID3D11Buffer>       m_vertexBuffer;
	ComPtr<ID3D11Buffer>       m_indexBuffer;
	
	UINT                       m_indexCount;
	UINT                       m_vertexCount;

	ComPtr<ID3D11Buffer>       m_tessFactorsHS;
	TessellationFactors        m_tessellationFactors;

	UINT                       m_resolution;
	float                      m_heightMultiplier;
};