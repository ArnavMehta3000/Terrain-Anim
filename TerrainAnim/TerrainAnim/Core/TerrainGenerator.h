#pragma once

class Terrain;

class TerrainGenerator
{
public:
	static void GenerateRandomPointOnTerrain(Terrain* terrain, Vector2& a, Vector2& b);

	static void DoSmoothFIR(Terrain* terrain, float filterSize);
	static void DoSmoothGaussian(Terrain* terrain, float strength);
	
	static void NormalizeHeight(Terrain* terrain, float minHeight, float maxHeight);
	static void RecalculateNormals(Terrain* terrain);

	static void FaultFormation(Terrain* terrain, int iterations, float minHeight, float maxHeight);
	static void DiamondSquare(Terrain* terrain, float heightMultiplier, float roughness, float roughnessDemultiplier);
	static void Voxelize(Terrain* terrain, float voxelSize);
	static void ParticleDeposition(Terrain* terrain, int iterations);
};