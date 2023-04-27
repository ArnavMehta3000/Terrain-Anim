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

void TerrainGenerator::RecaluclateNormals(Terrain* terrain)
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
	RecaluclateNormals(terrain);
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
	RecaluclateNormals(terrain);
	terrain->UpdateBuffers();
}

void TerrainGenerator::Voxelize(Terrain* terrain, float voxelSize)
{
	for (int i = 0; i < terrain->m_terrainVertices.size(); i++)
	{
		// Round the vertex position to the nearest multiple of the voxel size
		terrain->GetVertices()[i].Pos.x = std::round(terrain->GetVertices()[i].Pos.x / voxelSize) * voxelSize;
		terrain->GetVertices()[i].Pos.y = std::round(terrain->GetVertices()[i].Pos.y / voxelSize) * voxelSize;
		terrain->GetVertices()[i].Pos.z = std::round(terrain->GetVertices()[i].Pos.z / voxelSize) * voxelSize;
	}
	
	terrain->UpdateBuffers();
}

void TerrainGenerator::ParticleDeposition(Terrain* terrain, int iterations)
{
	// Assumes the grid is already flattened

	auto width  = terrain->m_heightMap->GetWidth();
	auto height = terrain->m_heightMap->GetHeight();

	constexpr int erosionRadius  = 3;
	float inertia                = 0.05f;  // Should be between 0-1
	float sedimentCapacityFactor = 4;
	float minSedimentCapacity    = 0.01f;

	float erodeSpeed             = 0.3f;   // Should be between 0-1
	float depositSpeed            = 0.3f;   // Should be between 0-1
	float evaporateSpeed         = 0.01f;  // Should be between 0-1
	float gravity                = 4.0f;
	
	int maxDropletLifeTime       = 30;
	float initialWaterVolume     = 1;
	float initialSpeed            = 1;

	std::vector<std::vector<int>> erosionBrushIndices;
	std::vector<std::vector<float>> erosionBrushWeights;

	int currentErosionRadius;

	struct HeightAndGradient
	{
		float height;
		float gradientX;
		float gradientY;
	};


	// Helper Lambdas
	auto InitBrushIndices = [&]()
	{
		erosionBrushIndices.resize(width * height);
		erosionBrushWeights.resize(width * height);

		std::array<int, erosionRadius * erosionRadius * 4> xOffsets;
		std::array<int, erosionRadius * erosionRadius * 4> yOffsets;
		std::array<int, erosionRadius* erosionRadius * 4> weights;

		float weightSum = 0.0f;
		int addIndex = 0;

		for (int i = 0; i < erosionBrushIndices[0].size(); i++)
		{
			int centreX = i % width;
			int centreY = i / height;

			if (centreY <= erosionRadius || 
				centreY >= width - erosionRadius ||
				centreX <= erosionRadius + 1 || 
				centreX >= height - erosionRadius)
			{
				weightSum = 0;

				addIndex = 0;
				for (int y = -erosionRadius; y <= erosionRadius; y++)
				{
					for (int x = -erosionRadius; x <= erosionRadius; x++) 
					{
						float sqrDst = (float)(x * x + y * y);
						if (sqrDst < erosionRadius * erosionRadius)
						{
							int coordX = centreX + x;
							int coordY = centreY + y;

							if (coordX >= 0 && coordX < width && coordY >= 0 && coordY < height)
							{								
								float weight = 1 - std::sqrtf(sqrDst) / erosionRadius;
								weightSum += weight;

								weights[addIndex]  = weight;
								xOffsets[addIndex] = x;
								yOffsets[addIndex] = y;
								addIndex++;
							}
						}
					}
				}
			}

			int numEntries = addIndex;
			erosionBrushIndices[i].resize(numEntries);
			erosionBrushWeights[i].resize(numEntries);

			for (int j = 0; j < numEntries; j++)
			{
				erosionBrushIndices[i][j] = (yOffsets[j] + centreY) * width + xOffsets[j] + centreX;
				erosionBrushWeights[i][j] = weights[j] / weightSum;
			}
		}
	};

	auto CalculateHeightAndGradient = [&](float posX, float posY)
	{
		int coordX = (int)posX;
		int coordY = (int)posY;

		// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
		float x = posX - coordX;
		float y = posY - coordY;

		// Calculate heights of the four nodes of the droplet's cell
		int nodeIndexNW = coordY * width + coordX;
		float heightNW = terrain->GetVertices()[nodeIndexNW].Pos.y;
		float heightNE = terrain->GetVertices()[nodeIndexNW + 1].Pos.y;
		float heightSW = terrain->GetVertices()[nodeIndexNW + width].Pos.y;
		float heightSE = terrain->GetVertices()[nodeIndexNW + width+ 1].Pos.y;

		// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
		float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
		float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

		// Calculate height with bilinear interpolation of the heights of the nodes of the cell
		float newHeight = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;


		HeightAndGradient output
		{
			.height = newHeight,
			.gradientX = gradientX,
			.gradientY = gradientY
		};
		return output;
	};
	// Helper Lambdas

	

	// Initialize
	currentErosionRadius = erosionRadius;
	InitBrushIndices();


	// Do erosion

	for (int iteration = 0; iteration < iterations; iteration++) 
	{
		// Create water droplet at random point on map
		float posX = GetRandomFloat(0, width - 1);
		float posY = GetRandomFloat(0, height  - 1);
		float dirX = 0;
		float dirY = 0;
		float speed = initialSpeed;
		float water = initialWaterVolume;
		float sediment = 0;

		for (int lifetime = 0; lifetime < maxDropletLifeTime; lifetime++)
		{
			auto nodeX = (int)posX;
			auto nodeY = (int)posY;

			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
			int dropletIndex  = nodeY * width + nodeX;
			float cellOffsetX = posX - nodeX;
			float cellOffsetY = posY - nodeY;

			// Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
			HeightAndGradient heightAndGradient = CalculateHeightAndGradient(posX, posY);

			// Update the droplet's direction and position (move position 1 unit regardless of speed)
			dirX = (dirX * inertia - heightAndGradient.gradientX * (1 - inertia));
			dirY = (dirY * inertia - heightAndGradient.gradientY * (1 - inertia));
			// Normalize direction
			float len = std::sqrtf(dirX * dirX + dirY * dirY);
			if (len != 0) 
			{
				dirX /= len;
				dirY /= len;
			}
			posX += dirX;
			posY += dirY;

			// Stop simulating droplet if it's not moving or has flowed over edge of map
			if ((dirX == 0 && dirY == 0) || 
				posX < 0 || 
				posX >= width - 1 || 
				posY < 0 || 
				posY >= height - 1) 
				break;

			// Find the droplet's new height and calculate the deltaHeight
			float newHeight   = CalculateHeightAndGradient(posX, posY).height;
			float deltaHeight = newHeight - heightAndGradient.height;

			// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
			float sedimentCapacity = std::max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

			// If carrying more sediment than capacity, or if flowing uphill:
			if (sediment > sedimentCapacity || deltaHeight > 0)
			{
				// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
				float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
				sediment -= amountToDeposit;

				// Add the sediment to the four nodes of the current cell using bilinear interpolation
				// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
				terrain->GetVertices()[dropletIndex].Pos.y               += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
				terrain->GetVertices()[dropletIndex + 1].Pos.y           += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
				terrain->GetVertices()[dropletIndex + width].Pos.y       += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
				terrain->GetVertices()[dropletIndex + width + 1].Pos.y   += amountToDeposit * cellOffsetX * cellOffsetY;

			}
			else
			{
				// Erode a fraction of the droplet's current carry capacity.
				// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
				float amountToErode = std::min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);

				// Use erosion brush to erode from all nodes inside the droplet's erosion radius
				for (int brushPointIndex     = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
					int nodeIndex            = erosionBrushIndices[dropletIndex][brushPointIndex];
					float weighedErodeAmount = amountToErode * erosionBrushWeights[dropletIndex][brushPointIndex];
					float deltaSediment      = (terrain->GetVertices()[nodeIndex].Pos.y < weighedErodeAmount) ? terrain->GetVertices()[nodeIndex].Pos.y : weighedErodeAmount;
					
					terrain->GetVertices()[nodeIndex].Pos.y -= deltaSediment;
					sediment       += deltaSediment;
				}
			}

			// Update droplet's speed and water content
			speed = std::sqrtf(speed * speed + deltaHeight * gravity);
			water *= (1 - evaporateSpeed);
		}
	}

}
