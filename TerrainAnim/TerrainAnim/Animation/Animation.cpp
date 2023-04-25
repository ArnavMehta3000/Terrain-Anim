#include "pch.h"
#include "Animation/Animation.h"
#include "Graphics/Direct3D.h"

void Mesh::GeneratePrimitiveBuffers()
{
	for (auto& primitive : Primitives)
	{
		// Create vertex buffer
		{
			// Get vertices list
			std::vector<SimpleVertex> verts;
			std::for_each(primitive->VertexData.begin(), primitive->VertexData.end(),
				[&verts](SkinnedVertexData data)
				{
					verts.push_back(data.Vertex);
				});

			CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
			vbd.Usage = D3D11_USAGE_DYNAMIC;
			vbd.ByteWidth = sizeof(SimpleVertex) * (UINT)verts.size();
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
			vertexInitData.pSysMem = verts.data();
			HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, primitive->m_vertexBuffer.ReleaseAndGetAddressOf()));
		}


		// Create index buffer
		{
			CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
			ibd.Usage = D3D11_USAGE_DEFAULT;
			ibd.ByteWidth = sizeof(int) * (UINT)primitive->Indices.size();
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;

			CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
			indexInitData.pSysMem = primitive->Indices.data();
			HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, primitive->m_indexBuffer.ReleaseAndGetAddressOf()));
		}

		primitive->m_vertexCount = static_cast<UINT>(primitive->VertexData.size());
		primitive->m_indexCount  = static_cast<UINT>(primitive->Indices.size());
	}
}
