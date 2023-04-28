#include "pch.h"
#include "Entities/Terrain.h"
#include "TerrainGenerator.h"
#include <random>
#include <numbers>

static float GetRandomFloat(float min = -1.0f, float max = 1.0f)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

void TerrainGenerator::GenerateRandomPointOnTerrain(Terrain* terrain, Vector2& a, Vector2& b)
{
	float width  = (float)terrain->m_heightMap->GetWidth();
	float height = (float)terrain->m_heightMap->GetHeight();

	a.x = GetRandomFloat(0.0f, width);
	a.y = GetRandomFloat(0.0f, height);

	int count = 0;

	do
	{
		b.x = GetRandomFloat(0.0f, width);
		b.y = GetRandomFloat(0.0f, height);

		if (count++ == 1000)
		{
			// Endless loop
			assert(false);
		}

	} while (a == b);
}

void TerrainGenerator::DoSmoothFIR(Terrain* terrain, float filterSize)
{
	auto Sinc = [](float x)
	{
		if (fabs(x) < 1e-6)
			return 1.0f;

		auto output = static_cast<float>(std::sin(x * std::numbers::pi) / (x * std::numbers::pi));
		return output;
	};

	auto Window = [](float x, int size)
	{
		// Blackman-Nuttall window
		float a0 = 0.3635819f;
		float a1 = 0.4891775f;
		float a2 = 0.1365995f;
		float a3 = 0.0106411f;
		float t = 2.0f * x / ((float)size - 1.0f);

		auto output = a0 - a1 * cos(2.0f * std::numbers::pi * t) +
			a2 * cos(4.0f * std::numbers::pi * t) -
			a3 * cos(6.0f * std::numbers::pi * t);

		return (float)(output);
	};

	auto width = terrain->m_heightMap->GetWidth();
	auto height = terrain->m_heightMap->GetHeight();
	

	// Compute filet coefficients
	std::vector<float> kernel;
	auto kernelSize = static_cast<int>(2.0f * filterSize + 1.0f);
	kernel.resize(kernelSize);
	float sum = 0.0f;
	for (int i = 0; i < kernelSize; i++)
	{
		float x = static_cast<float>(i) - filterSize;
		kernel[i] = Sinc(x) * Window(x, kernelSize);
		sum += kernel[i];
	}
	// Normalize filter coefficients
	std::ranges::for_each(kernel, [sum](float& k) {k /= sum; });


	// Apply FIR
	for (int z = 0; z < height; z++)
	{
		for (int x = 0; x < width; x++)
		{
			float newHeight = 0.0f;

			for (int i = 0; i < kernelSize; i++)
			{
				int index = std::clamp(x + i - kernelSize / 2, 0, width - 1) + 
							std::clamp(z + i - kernelSize / 2, 0, height - 1) * width;

				newHeight += kernel[i] * terrain->GetVertices()[index].Pos.y;
			}
			terrain->GetVertices()[x + z * width].Pos.y = newHeight;
		}
	}
	// Sweep the FIR algorithm in other directions
//#define MULTI_SWEEP

#ifdef MULTI_SWEEP
	for (int z = 0; z < height; z++)
	{
		for (int x = width - 1; x >= 0; x--)
		{
			float newHeight = 0.0f;

			for (int i = 0; i < kernelSize; i++)
			{
				int index = std::clamp(x + i - kernelSize / 2, 0, width - 1) +
					std::clamp(z + i - kernelSize / 2, 0, height - 1) * width;

				newHeight += kernel[i] * terrain->GetVertices()[index].Pos.y;
			}
			terrain->GetVertices()[x + z * width].Pos.y = newHeight;
		}
	}

	for (int z = height - 1; z >= 0; z--)
	{
		for (int x = 0; x < width; x++)
		{
			float newHeight = 0.0f;

			for (int i = 0; i < kernelSize; i++)
			{
				int index = std::clamp(x + i - kernelSize / 2, 0, width - 1) +
					std::clamp(z + i - kernelSize / 2, 0, height - 1) * width;

				newHeight += kernel[i] * terrain->GetVertices()[index].Pos.y;
			}
			terrain->GetVertices()[x + z * width].Pos.y = newHeight;
		}
	}

	for (int z = height - 1; z >= 0; z--)
	{
		for (int x = width - 1; x >= 0; x--)
		{
			float newHeight = 0.0f;

			for (int i = 0; i < kernelSize; i++)
			{
				int index = std::clamp(x + i - kernelSize / 2, 0, width - 1) +
					std::clamp(z + i - kernelSize / 2, 0, height - 1) * width;

				newHeight += kernel[i] * terrain->GetVertices()[index].Pos.y;
			}
			terrain->GetVertices()[x + z * width].Pos.y = newHeight;
		}
	}
#endif // MULTI_SWEEP


	terrain->UpdateBuffers();
}

void TerrainGenerator::DoSmoothGaussian(Terrain* terrain, float strength)
{
	int width = terrain->m_heightMap->GetWidth();
	int height = terrain->m_heightMap->GetHeight();

	// Create a copy of the heightmap to use as a buffer
	std::vector<float> newHeights(width * height);

	// Apply filter
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float sum = 0.0f;
			float count = 0.0f;

			// Calculate the sum of heights in the 3x3 neighborhood
			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					int nx = x + dx;
					int ny = y + dy;

					if (nx >= 0 && nx < width && ny >= 0 && ny < height)
					{
						sum += terrain->GetVertices()[nx * width + ny].Pos.y;
						count += 1.0f;
					}
				}
			}

			// Calculate the average height and set the new height value in the newHeights vector
			float avg = sum / count;
			int index = x * width + y;
			newHeights[index] = terrain->GetVertices()[index].Pos.y * (1.0f - strength) + avg * strength;
		}
	}

	for (int i = 0; i < width * height; i++)
	{
		terrain->GetVertices()[i].Pos.y = newHeights[i];
	}

	newHeights.clear();

	// Update the terrain
	NormalizeHeight(terrain, 0.0f, 100.0f);
	terrain->UpdateBuffers();
}

void TerrainGenerator::NormalizeHeight(Terrain* terrain, float minHeight, float maxHeight)
{
	float minY = std::ranges::min(terrain->GetVertices(), {}, [](const SimpleVertex& v) { return v.Pos.y; }).Pos.y;
	float maxY = std::ranges::max(terrain->GetVertices(), {}, [](const SimpleVertex& v) { return v.Pos.y; }).Pos.y;

	float delta = maxY - minY;
	float range = maxHeight - minHeight;

	for (int i = 0; i < terrain->GetVertices().size(); i++)
	{
		terrain->GetVertices()[i].Pos.y = ((terrain->GetVertices()[i].Pos.y - minY) / delta) * range + minHeight;
	}
}

void TerrainGenerator::RecalculateNormals(Terrain* terrain)
{
	for (int i = 0; i < terrain->m_indexCount; i += 3)
	{
		auto index0 = terrain->m_terrainIndices[i];
		auto index1 = terrain->m_terrainIndices[i + 1];
		auto index2 = terrain->m_terrainIndices[i + 2];

		auto v1 = terrain->GetVertices()[index1].Pos - terrain->GetVertices()[index0].Pos;
		auto v2 = terrain->GetVertices()[index2].Pos - terrain->GetVertices()[index0].Pos;

		auto normal = v1.Cross(v2);
		normal.Normalize();

		terrain->GetVertices()[index0].Normal += normal;
		terrain->GetVertices()[index1].Normal += normal;
		terrain->GetVertices()[index2].Normal += normal;
	}

	for (int i = 0; i < terrain->GetVertices().size(); i++)
	{
		terrain->GetVertices()[i].Normal.Normalize();
	}
}

void TerrainGenerator::FaultFormation(Terrain* terrain, int iterations, float minHeight, float maxHeight)
{
	float deltaHeight = maxHeight - minHeight;

	for (int i = 0; i < iterations; i++)
	{
		float iterRatio = ((float)i / (float)iterations);
		float height = maxHeight - iterRatio * deltaHeight;

		Vector2 pointA = Vector2::Zero;
		Vector2 pointB = Vector2::Zero;

		GenerateRandomPointOnTerrain(terrain, pointA, pointB);

		float deltaX = pointB.x - pointA.x;
		float deltaZ = pointB.y - pointA.y;


		for (int z = 0; z < terrain->m_heightMap->GetHeight(); z++)
		{
			for (int x = 0; x < terrain->m_heightMap->GetWidth(); x++)
			{
				float dirX = (float)x - pointA.x;
				float dirZ = (float)z - pointA.y;

				float cross = dirX * deltaZ - deltaX * dirZ;

				if (cross > 0)
				{
					int index = z * terrain->m_heightMap->GetWidth() + x;
					// Update height vertex
					terrain->GetVertices()[index].Pos.y += height;
				}
			}
		}
	}

	NormalizeHeight(terrain, minHeight, maxHeight);
	RecalculateNormals(terrain);
	terrain->UpdateBuffers();
}

void TerrainGenerator::DiamondSquare(Terrain* terrain, float heightMultiplier, float roughness, float roughnessDemultiplier)
{
	int gridSize = terrain->m_heightMap->GetWidth();

	// Set the four corners with starting values
	terrain->GetVertices()[0].Pos.y                         = GetRandomFloat() * roughness;
	terrain->GetVertices()[gridSize - 1].Pos.y              = GetRandomFloat() * roughness;
	terrain->GetVertices()[gridSize * (gridSize - 1)].Pos.y = GetRandomFloat() * roughness;
	terrain->GetVertices()[gridSize * gridSize - 1].Pos.y   = GetRandomFloat() * roughness;


	for (int sideLength = gridSize - 1; sideLength >= 2; sideLength /= 2)
	{
		int halfSide = sideLength / 2;
		float scale = sideLength * roughness;

		// Diamond Step
		for (int x = 0; x < gridSize - 1; x += sideLength)
		{
			for (int y = 0; y < gridSize - 1; y += sideLength)
			{
				float h1 = terrain->GetVertices()[x + y * gridSize].Pos.y;                                // Top left
				float h2 = terrain->GetVertices()[x + (y + sideLength) * gridSize].Pos.y;                 // Top right
				float h3 = terrain->GetVertices()[x + (y + sideLength) * gridSize].Pos.y;                 // Bottom left
				float h4 = terrain->GetVertices()[(x + sideLength) + (y + sideLength) * gridSize].Pos.y;  // Bottom right

				float avg = (h1 + h2 + h3 + h4) / 4.0f;

				// Set middle value
				int index = x + halfSide + (y + halfSide) * gridSize;
				float randFloat = GetRandomFloat(-scale, scale);
				terrain->GetVertices()[index].Pos.y = avg + randFloat * heightMultiplier;
			}
		}


		// Square Step
		for (int x = 0; x < gridSize - 1; x += halfSide)
		{
			for (int y = 0; y < gridSize - 1; y += halfSide)
			{
				float h1 = terrain->GetVertices()[x + y * gridSize].Pos.y;                           // Top left
				float h2 = terrain->GetVertices()[x + (y + halfSide) * gridSize].Pos.y;              // Bottom left
				float h3 = terrain->GetVertices()[(x + halfSide) + y * gridSize].Pos.y;              // Top right
				float h4 = terrain->GetVertices()[(x + halfSide) + (y + halfSide) * gridSize].Pos.y; // Bottom right

				float avg = (h1 + h2 + h3 + h4) / 4.0f;

				// Set middle value
				int index = x + halfSide + (y + halfSide) * gridSize;
				float randValue = GetRandomFloat(-scale, scale);
				terrain->GetVertices()[index].Pos.y = avg + randValue * heightMultiplier;
			}
		}

		roughness *= roughnessDemultiplier;
	}
	NormalizeHeight(terrain, 0.0f, 100.0f);
	RecalculateNormals(terrain);
	terrain->UpdateBuffers();
}

void TerrainGenerator::Voxelize(Terrain* terrain, float voxelSize)
{
	for (int i = 0; i < terrain->m_terrainVertices.size(); i++)
	{
		// Round the vertex position to the nearest multiple of the voxel size
		terrain->GetVertices()[i].Pos.x = std::floorf(terrain->GetVertices()[i].Pos.x / voxelSize) * voxelSize;
		terrain->GetVertices()[i].Pos.y = std::floorf(terrain->GetVertices()[i].Pos.y / voxelSize) * voxelSize;
		terrain->GetVertices()[i].Pos.z = std::floorf(terrain->GetVertices()[i].Pos.z / voxelSize) * voxelSize;
	}
	
	terrain->UpdateBuffers();
}

void TerrainGenerator::ParticleDeposition(Terrain* terrain, int iterations, float radius, float depositAmount)
{
	auto width  = terrain->m_heightMap->GetWidth();
	auto height = terrain->m_heightMap->GetHeight();



	for (int i = 0; i < iterations; i++)
	{
		// Choose a random position on the terrain grid
		float xPos = GetRandomFloat(0.0f, (float)width);
		float yPos = GetRandomFloat(0.0f, (float)height);
		
		// Create a new particle with a random direction
		float direction = GetRandomFloat();
		float xDir = std::cosf(direction);
		float yDir = std::sinf(direction);

		// Loop through all vertices and update their heights based on the particle's position
		for (int j = 0; j < terrain->m_vertexCount; j++) 
		{
			float dist = std::sqrtf(
				std::powf(terrain->GetVertices()[j].Pos.x - xPos, 2.0f) + 
				std::powf(terrain->GetVertices()[j].Pos.z - yPos, 2.0f));

			// If the vertex is within the particle's radius, update its height
			if (dist < radius)
			{
				float deposit = depositAmount * (1 - dist / radius);
				terrain->GetVertices()[j].Pos.y += deposit;
			}
		}

		// Move the particle in its direction
		xPos += xDir;
		yPos += yDir;
	}


	RecalculateNormals(terrain);
	NormalizeHeight(terrain, 0.0f, 100.0f);
	terrain->UpdateBuffers();
}
