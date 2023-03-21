#include "pch.h"
#include "DiamondSqrGrid.h"
#include "Graphics/Direct3D.h"
#include  <random>

float RandFloat()
{
	static std::mt19937 gen(0);
	static std::uniform_real_distribution<float> dist(-1.f, 1.f);
	return dist(gen);
}


DiamondSqrGrid::DiamondSqrGrid(UINT resolution)
	:
	m_resolution(resolution),
	m_heightMultiplier(1.0f)
	
{
	ZeroMemory(&m_tessellationFactors, sizeof(TessellationFactors));
	m_tessellationFactors.EdgeTessFactor   = 1.0f;
	m_tessellationFactors.InsideTessFactor = 1.0f;

	Shader::InitInfo desc{};
	desc.VertexShaderFile = L"Shaders/Grid/Grid_VS.hlsl";
	desc.PixelShaderFile  = L"Shaders/Grid/Grid_PS.hlsl";
	desc.HullShaderFile   = L"Shaders/Grid/Grid_HS.hlsl";
	desc.DomainShaderFile = L"Shaders/Grid/Grid_DS.hlsl";
	m_shader              = std::make_unique<Shader>(desc);

	std::vector<SimpleVertex> v;
	v = CreateFlatGrid();
	DiamondSqrGrid(v);
}

DiamondSqrGrid::~DiamondSqrGrid()
{
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
}

void DiamondSqrGrid::Update(float dt, const InputEvent& input)
{
	// Nothing to update yet
}

void DiamondSqrGrid::Render()
{
	m_shader->BindAll();

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	D3D_CONTEXT->HSSetConstantBuffers(0, 1, m_tessFactorsHS.GetAddressOf());
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}

void DiamondSqrGrid::GUI()
{
	if (ImGui::TreeNode("DS-grid Settings"))
	{
		ImGui::DragFloat("Multiplier", &m_heightMultiplier, 0.1f, 0.0f);
	}
}

std::vector<SimpleVertex> DiamondSqrGrid::CreateFlatGrid() const 
{
	auto size        = (UINT)GetGridSize();
	auto vertexCount = UINT(size * size);

	float halfSize = 0.5f * size;

	float dx = size / (size - 1.0f);
	float dz = size / (size - 1.0f);

	float du = 1.0f / (size - 1.0f);
	float dv = 1.0f / (size - 1.0f);

	std::vector<SimpleVertex> vertices(vertexCount);
	// Generate vertices
	for (UINT row = 0; row < size; row++)
	{
		float z = halfSize - (float)row * dz;
		for (UINT col = 0; col < size; col++)
		{
			float x = -halfSize + (float)col * dx;

			vertices[row * size + col] = { Vector3(x, 0.0f, z),
										   Vector3(0.0f, 1.0f, 0.0f),
										   Vector2((float)row * du, (float)col * dv) };
		}
	}

	return vertices;
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
