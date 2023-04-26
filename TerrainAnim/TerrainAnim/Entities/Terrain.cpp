#include "pch.h"
#include "Entities/Terrain.h"
#include "Graphics/Direct3D.h"


Terrain::Terrain()
	:
	Entity(),
	m_texDirt0(L"Assets/Textures513/darkdirt.png"),
	m_texDirt1(L"Assets/Textures513/lightdirt.png"),
	m_texStone(L"Assets/Textures513/stone.png"),
	m_texGrass(L"Assets/Textures513/grass.png"),
	m_texSnow(L"Assets/Textures513/snow.png"),
	m_vertexCount(0),
	m_faceCount(0),
	m_indexCount(0),
	m_isHeightMapApplied(false)
{
	m_tessellationFactors.EdgeTessFactor = 1.0f;
	m_tessellationFactors.InsideTessFactor = 1.0f;
	m_gradients = TerrainGradients{};

	Shader::InitInfo desc{};
	desc.VertexShaderFile = L"Shaders/Grid/Grid_VS.hlsl";
	desc.PixelShaderFile  = L"Shaders/Grid/Grid_PS.hlsl";
	desc.HullShaderFile   = L"Shaders/Grid/Grid_HS.hlsl";
	desc.DomainShaderFile = L"Shaders/Grid/Grid_DS.hlsl";
	m_shader              = std::make_unique<Shader>(desc);

	D3D->CreateConstantBuffer(m_tessFactorsHS, sizeof(TessellationFactors));
	D3D->CreateConstantBuffer(m_gradientBuffer, sizeof(TerrainGradients));
}

Terrain::~Terrain() = default;


void Terrain::LoadFile(const std::string& filename, int width, int height)
{
	if (m_heightMap == nullptr)
	{
		m_heightMap = std::make_unique<HeightMap>(filename.c_str(), width, height);

		m_vertexCount = m_heightMap->GetWidth() * m_heightMap->GetHeight();
		m_faceCount   = (m_heightMap->GetWidth() - 1) * (m_heightMap->GetHeight() - 1) * 2;  // 2 triangles
		m_indexCount  = m_faceCount * 3;
	}
	else
	{
		// Check if the heightmap has the same file name
		if (m_heightMap->GetFileName().compare(filename) != 0)
		{
			m_heightMap.reset();
			m_heightMap = std::make_unique<HeightMap>(filename.c_str(), width, height);
		
			m_vertexCount = m_heightMap->GetWidth() * m_heightMap->GetHeight();
			m_faceCount   = (m_heightMap->GetWidth() - 1) * (m_heightMap->GetHeight() - 1) * 2;  // 2 triangles
			m_indexCount  = m_faceCount * 3;
		}
		else
		{
			// Requested height map and exiting heightmap are the same
			// Ignore creation
			return;
		}
	}
	GenerateBuffers();
}


void Terrain::Update(float dt, const InputEvent& input)
{
	if (m_heightMap == nullptr)
		return;

	Entity::Update(dt, input);

	D3D_CONTEXT->UpdateSubresource(m_tessFactorsHS.Get(), 0, nullptr, &m_tessellationFactors, 0, 0);
	D3D_CONTEXT->UpdateSubresource(m_gradientBuffer.Get(), 0, nullptr, &m_gradients, 0, 0);
}

void Terrain::Render()
{
	if (m_heightMap == nullptr)
		return;

	m_shader->BindAll();
	m_texDirt0.SetInPS(0);
	m_texDirt1.SetInPS(1);
	m_texStone.SetInPS(2);
	m_texGrass.SetInPS(3);
	m_texSnow.SetInPS(4);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	D3D_CONTEXT->HSSetConstantBuffers(0, 1, m_tessFactorsHS.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(0, 1, m_gradientBuffer.GetAddressOf());
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}

void Terrain::GUI()
{
	if (m_heightMap == nullptr)
		return;

	if (ImGui::Button("Flatten"))
		Flatten();

	ImGui::SameLine();

	if (ImGui::Button("Apply Heightmap"))
		ApplyHeightmap();

	ImGui::Separator();

	ImGui::DragFloat("Low Dirt", &m_gradients.Dirt0Height);
	ImGui::DragFloat("High Dirt", &m_gradients.Dirt1Height);
	ImGui::DragFloat("Stone", &m_gradients.StoneHeight);
	ImGui::DragFloat("Grass", &m_gradients.GrassHeight);
	ImGui::DragFloat("Snow", &m_gradients.SnowHeight);
}


void Terrain::Flatten()
{
	std::ranges::for_each(m_terrainVertices,
		[](SimpleVertex& vertex)
		{
			vertex.Pos.y = 0.0f;
		});

	UpdateBuffers();
	m_isHeightMapApplied = false;
}

void Terrain::ApplyHeightmap()
{
	if (m_isHeightMapApplied)
		return;  // Already applied, no need to waste resources

	int width = m_heightMap->GetWidth();
	int height = m_heightMap->GetHeight();

	int index = 0;
	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < height; z++)
		{
			assert(index < m_terrainVertices.size());

			float y = m_heightMap->GetValue(width * x + z) * 100.0f;

			m_terrainVertices[index].Pos.y = y;

			index++;
		}
	}

	UpdateBuffers();
	m_isHeightMapApplied = true;
}


void Terrain::GenerateBuffers()
{
	auto CreateVertex = [](float x, float y, float z, float width, float height)
	{
		float texU = x / width;
		float texV = z / height;
		SimpleVertex vert
		{
			vert.Pos      = Vector3(x, y, z),
			vert.Normal   = Vector3(0.0f, 1.0f, 0.0f),
			vert.TexCoord = Vector2(texU, texV)
		};
		return vert;
	};

	assert(m_heightMap);
	m_terrainVertices.resize(m_vertexCount);
	
	int width  = m_heightMap->GetWidth();
	int height = m_heightMap->GetHeight();

	// Create vertices
	int index  = 0;
	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < height; z++)
		{
			assert(index < m_terrainVertices.size());

			float y = m_heightMap->GetValue(width * x + z) * 100.0f;
			
			m_terrainVertices[index] = CreateVertex((float)x, y, (float)z, (float)width, (float)height);
			
			index++;
		}
	}

	std::vector<UINT> indices(m_indexCount);  // 3 vertices per face

	int tris = 0;
	for (int x = 0; x < width - 1; x++)
	{
		for (int z = 0; z < height - 1; z++)
		{
			indices[tris]     = x * height + z;
			indices[tris + 1] = x * height + z + 1;
			indices[tris + 2] = (x + 1) * height + z;
			indices[tris + 3] = (x + 1) * height + z;
			indices[tris + 4] = x * height + z + 1;
			indices[tris + 5] = (x + 1) * height + z + 1;

			// Next quad
			tris += 6;
		}
	}



	// ----- CREATE D3D BUFFERS -----
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage          = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth      = sizeof(SimpleVertex) * m_vertexCount;
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vsd);
	vsd.pSysMem = m_terrainVertices.data();
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vsd, m_vertexBuffer.ReleaseAndGetAddressOf()))

	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage          = D3D11_USAGE_DYNAMIC;
	ibd.ByteWidth      = sizeof(UINT) * (m_faceCount * 3);
	ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, isd);
	isd.pSysMem = indices.data();

	HR(D3D_DEVICE->CreateBuffer(&ibd, &isd, m_indexBuffer.ReleaseAndGetAddressOf()))	
	// ----- CREATE D3D BUFFERS -----
}

void Terrain::UpdateBuffers()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D_CONTEXT->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto* bufferData = reinterpret_cast<SimpleVertex*>(mappedResource.pData);
	for (int i = 0; i < m_terrainVertices.size(); ++i)
	{
		bufferData[i] = m_terrainVertices[i];
	}

	D3D_CONTEXT->Unmap(m_vertexBuffer.Get(), 0);
}
