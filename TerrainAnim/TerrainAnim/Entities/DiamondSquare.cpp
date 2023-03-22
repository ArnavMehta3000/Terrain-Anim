#include "pch.h"
#include "Entities/DiamondSquare.h"
#include <random>

float RandFloat(float rangeMin = -1.0f, float rangeMax = 1.0f)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(rangeMin, rangeMax);
	return dis(gen);
}

DiamondSquare::DiamondSquare()
	:
	GridEntity(9),
	m_roughness(1.0f),
	m_roughnessDemultiplier(0.5f)
{
	// Grid's constructor will create a flat grid, VB and IB
}

DiamondSquare::~DiamondSquare()
{
}

void DiamondSquare::Update(float dt, const InputEvent& input)
{
	GridEntity::Update(dt, input);
}

void DiamondSquare::Render()
{
	GridEntity::Render();
}

static bool needsReset = false;
#define CHECK_RESET() if (ImGui::IsItemEdited()) needsReset = true

void DiamondSquare::GUI()
{
	GridEntity::GUI();

	ImGui::DragInt("Resolution", &m_resolution, 1.0f, 2, 11, "%d", ImGuiSliderFlags_NoInput);
	CHECK_RESET();
	ImGui::DragFloat("Height Multiplier", &m_multiplier, 0.1f, 1.0f);
	CHECK_RESET();
	ImGui::DragFloat("Starting Roughness", &m_roughness, 0.1f, 0.01f);
	CHECK_RESET();
	ImGui::DragFloat("Roughness Demultiplier", &m_roughnessDemultiplier, 0.01f, 0.01f, 0.9f);
	CHECK_RESET();

	if (!needsReset)
	{
		if (ImGui::Button("Generate Diamond Square Terrain"))
		{
			Generate();
			ApplyChanges();
		}
	}
	
	if (ImGui::Button("Reset"))
	{
		CreateFlatGrid();
		ApplyChanges();
		needsReset = false;
	}


}

void DiamondSquare::Generate()
{
	Timer timer;
	timer.Reset();
	timer.Start();

	float startingRoughness = m_roughness;
	auto gridSize = GetGridSize();
	
	// Set the 4 corners with starting values
	SetHeight(0, RandFloat() * m_roughness);
	SetHeight(gridSize - 1, RandFloat() * m_roughness);
	SetHeight(gridSize * (gridSize - 1), RandFloat() * m_roughness);
	SetHeight(gridSize * gridSize - 1, RandFloat() * m_roughness);
	

	// Diamond-Square Iterations
	for (int sideLength = gridSize - 1; sideLength >= 2; sideLength /=2)
	{
		int halfSide = sideLength / 2;
		float scale = sideLength * m_roughness;



		// Diamond Step
		for (int x = 0; x < gridSize - 1; x += sideLength)
		{
			for (int y = 0; y < gridSize - 1; y += sideLength)
			{
				float h1 = GetHeight(x + y * gridSize);                                // Top left
				float h2 = GetHeight(x + (y + sideLength) * gridSize);                 // Top right
				float h3 = GetHeight(x + (y + sideLength) * gridSize);                 // Bottom left
				float h4 = GetHeight((x + sideLength) + (y + sideLength) * gridSize);  // Bottom right

				float avg = (h1+h2+h3+h4) / 4.0f;

				// Set middle value
				SetHeight(x + halfSide + (y + halfSide) * gridSize, avg + RandFloat(-scale, scale) * m_multiplier);

			}
		}
		


		// Square Step
		for (int x = 0; x < gridSize - 1; x += halfSide)
		{
			for (int y = 0; y < gridSize - 1; y += halfSide)
			{
				float h1 = GetHeight(x + y * gridSize);                           // Top left
				float h2 = GetHeight(x + (y + halfSide) * gridSize);              // Bottom left
				float h3 = GetHeight((x + halfSide) + y * gridSize);              // Top right
				float h4 = GetHeight((x + halfSide) + (y + halfSide) * gridSize); // Bottom right

				float avg = (h1 + h2 + h3 + h4) / 4.0f;

				// Set middle value
				SetHeight(x + halfSide + (y + halfSide) * gridSize, avg + RandFloat(-scale, scale) * m_multiplier);
			}
		}
		
		m_roughness *= m_roughnessDemultiplier;
	}

	m_roughness = startingRoughness;
	ApplyChanges();

	timer.Stop();

	LOG("Diamond Square Generation took: " << timer.TotalTime() * 1000.0f << "ms");
}
