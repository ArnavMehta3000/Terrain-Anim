#pragma once
#include "Entities/GridEntity.h"

class DiamondSquare : public GridEntity
{
public:
	DiamondSquare();
	virtual ~DiamondSquare();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

private:
	// Generates terrain vertices based on diamond square algorithm
	virtual void Generate() override;

private:
	float m_roughness;
	float m_roughnessDemultiplier;
};