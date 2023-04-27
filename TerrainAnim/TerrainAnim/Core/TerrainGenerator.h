#pragma once

class Terrain;

class TerrainGenerator
{
public:
	static void GenerateRandomPointOnTerrain(Terrain* terrain, Vector2& a, Vector2& b);

	static void DoSmoothFIR(Terrain* terrain, float filterSize);
	
	static void NormalizeHeight(Terrain* terrain, float minHeight, float maxHeight);
	static void FaultFormation(Terrain* terrain, int iterations, float minHeight, float maxHeight);
};