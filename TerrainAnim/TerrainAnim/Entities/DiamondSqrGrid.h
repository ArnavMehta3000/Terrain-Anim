#pragma once
#include "Entities/GridEntity.h"

class DiamondSqrGrid : public GridEntity
{
public:
	explicit DiamondSqrGrid();
	virtual ~DiamondSqrGrid();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

	void DoDiamondSquareGrid(std::vector<SimpleVertex>& vertices);

private:
	std::unique_ptr<Shader>    m_shader;
};