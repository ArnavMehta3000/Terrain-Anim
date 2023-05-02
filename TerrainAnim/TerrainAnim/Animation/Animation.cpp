#include "pch.h"
#include "Animation/Animation.h"
#include "Graphics/Direct3D.h"

const Joint::JointPtr FindJointWithIndex(const Joint::JointPtr& rootJoint, const int index)
{
	if (rootJoint->Index == index)
		return rootJoint;

	for (auto& child : rootJoint->Children)
	{
		auto& joint = FindJointWithIndex(child, index);
		if (joint != nullptr)
			return joint;
	}

	// Joint not found
	return nullptr;
}

// https://github.com/HK416/glTF_Animation/blob/master/GLSL/anim_vert.glsl

void Mesh::GeneratePrimitiveBuffers() const
{
	for (const auto& primitive : Primitives)
	{
		// Create vertex buffer
		{
			// Set vertices list
			auto size = primitive->VertexData.size();
			std::vector<VSAnimInput> vsInput(size);

			// Multiply the vertex with the joint transform
			for (int i = 0; i < size; i++)
			{
				VSAnimInput input
				{
					.Pos          = primitive->VertexData[i].Vertex.Pos,
					.Normal       = primitive->VertexData[i].Vertex.Normal,
					.TexCoord     = primitive->VertexData[i].Vertex.TexCoord,
					.JointWeights = primitive->VertexData[i].Weight,
					.JointIndices = primitive->VertexData[i].Joint
				};

				vsInput[i] = input;
			}

			CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
			vbd.Usage          = D3D11_USAGE_DYNAMIC;
			vbd.ByteWidth      = sizeof(VSAnimInput) * (UINT)vsInput.size();
			vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
			vertexInitData.pSysMem = vsInput.data();
			HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, primitive->m_vertexBuffer.ReleaseAndGetAddressOf()));
		}


		// Create index buffer
		{
			CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
			ibd.Usage          = D3D11_USAGE_DEFAULT;
			ibd.ByteWidth      = sizeof(int) * (UINT)primitive->Indices.size();
			ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;

			CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
			indexInitData.pSysMem = primitive->Indices.data();
			HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, primitive->m_indexBuffer.ReleaseAndGetAddressOf()));
		}

		primitive->m_vertexCount = static_cast<UINT>(primitive->VertexData.size());
		primitive->m_indexCount  = static_cast<UINT>(primitive->Indices.size());
	}
}
