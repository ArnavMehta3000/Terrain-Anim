#include "pch.h"
#include "DiamondSqrGrid.h"
#include "Graphics/Direct3D.h"
#include  <random>

float RandFloat(float rangeMin = 1.0f, float rangeMax = 1.0f)
{
	static std::mt19937 gen(0);
	static std::uniform_real_distribution<float> dist(rangeMin, rangeMax);
	return dist(gen);
}


DiamondSqrGrid::DiamondSqrGrid()	
	:
	GridEntity()
{
	
}

DiamondSqrGrid::~DiamondSqrGrid()
{
	GridEntity::~GridEntity();
}

void DiamondSqrGrid::Update(float dt, const InputEvent& input)
{
	// Nothing to update yet
	GridEntity::Update(dt, input);
}

void DiamondSqrGrid::Render()
{
	GridEntity::Render();
}

void DiamondSqrGrid::GUI()
{
	GridEntity::GUI();

	if (ImGui::TreeNode("DS-grid Settings"))
	{
		ImGui::Text("Nothing here yet");

		ImGui::TreePop();
	}
}

void DiamondSqrGrid::DoDiamondSquareGrid(std::vector<SimpleVertex>& vertices)
{
	auto size = (int)GetGridSize();
	std::vector<std::vector<float>> heightMap(size, std::vector<float>(size, 0.f));

	// Init the corners with random values
	heightMap[0][0]               = RandFloat();
	heightMap[0][size - 1]        = RandFloat();
	heightMap[size - 1][0]        = RandFloat();
	heightMap[size - 1][size - 1] = RandFloat();

	float roughness = 0.5f;
	for (int i = 0; i < (int)m_resolution; i++)
	{
		auto stepSize = (int)std::pow(2, m_resolution - i);
		int halfStep = stepSize / 2;

		// Diamond step
		for (int y = halfStep; y < size; y += stepSize) 
		{
			for (int x = halfStep; x < size; x += stepSize) 
			{
				float sum = heightMap[y - halfStep][x - halfStep] +
							heightMap[y - halfStep][x + halfStep] +
							heightMap[y + halfStep][x - halfStep] +
							heightMap[y + halfStep][x + halfStep];
				
				heightMap[y][x] = sum / 4.f + RandFloat() * roughness;
			}
		}



		// Square step
		for (int y = 0; stepSize < size; y += stepSize)
		{
			for (int x = halfStep; x < size; x += stepSize) 
			{
				float sum = 0.f;
				int count = 0;

				if (y >= halfStep)       // Top neighbor
				{ 
					sum += heightMap[y - halfStep][x];
					count++;
				}
				if (y + halfStep < size) // Bottom neighbor
				{ 
					sum += heightMap[y + halfStep][x];
					count++;
				}
				if (x >= halfStep)       // Left neighbor
				{ 
					sum += heightMap[y][x - halfStep];
					count++;
				}
				if (x + halfStep < size) // Right neighbor
				{ 
					sum += heightMap[y][x + halfStep];
					count++;
				}
				
				heightMap[y][x] = sum / count + RandFloat() * roughness;
			}
		}

		// Reduce the roughness for the next iteration
		roughness *= 0.5f;
	}


	// Map heightmap to vertices
	for (auto& vert : vertices)
	{
		Vector3 pos = Vector3(vert.Pos.x, 0.0f, vert.Pos.z);
		
		int ix = (int)std::round((pos.x + 0.5f) * ((float)size - 1));
		int iz = (int)std::round((pos.z + 0.5f) * ((float)size - 1));

		float y = heightMap[iz][ix];
		pos.y = y;

		vert.Pos = pos;
	}
}
