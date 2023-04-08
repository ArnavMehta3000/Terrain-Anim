#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#include "GLTF.h"
#include <Core/Timer.h>
#include <assert.h>
#include "Graphics/Direct3D.h"

GLTF::GLTF()
    :
    m_scaleFactor(100.0f)
{
}

GLTF::~GLTF()
{
    m_model.~Model();
}

bool GLTF::Load(const char* filename)
{
    Timer loadTime;
    loadTime.Start();
    tinygltf::TinyGLTF loader;

    std::string err, warn;
    if (!loader.LoadASCIIFromFile(&m_model, &err, &warn, filename))
    {
        // Failed to load ASCII, try binary
        if (!loader.LoadBinaryFromFile(&m_model, &err, &warn, filename))
        {
            // Failed to load binary as well
            LOG("Failed to load GLTF");
            LOG("Warning: " << warn);
            LOG("Error: " << err);
            return false;
        }
    }
    if (!err.empty())
        LOG("Error: " << err);
    if (!warn.empty())
        LOG("Warning: " << warn);


    // ----- LOAD GLTF SCENE -----
    const tinygltf::Scene& scene = m_model.scenes[m_model.defaultScene];
    for (auto& node : m_model.nodes)
    {
        ProcessNode(node);
    }


    loadTime.Stop();
    LOG("Loaded file [" << filename << "] in " << loadTime.TotalTime() * 1000.0f << "ms");
    return true;
}

void GLTF::ProcessNode(tinygltf::Node& node)
{
    // Loop through meshes
    if (node.mesh >= 0)
    {
        // Get mesh from model using current node mesh index
        const auto& mesh = m_model.meshes[node.mesh];

        // Loop through mesh primitives
        for (const auto& primitive : mesh.primitives)
        {
            const auto& accessors   = m_model.accessors;
            const auto& bufferViews = m_model.bufferViews;
            const auto& buffers     = m_model.buffers;

            // Read vertex data
            const auto& positionAccessor   = accessors[primitive.attributes.at("POSITION")];
            const auto& positionBufferView = bufferViews[positionAccessor.bufferView];
            const auto& positionBuffer     = buffers[positionBufferView.buffer];
            const auto* positionData       = &positionBuffer.data[positionAccessor.byteOffset + positionBufferView.byteOffset];
            const auto positionCount       = positionAccessor.count;


            const auto& indexAccessor   = accessors[primitive.indices];
            const auto& indexBufferView = bufferViews[indexAccessor.bufferView];
            const auto& indexBuffer     = buffers[indexBufferView.buffer];
            const auto* indexData       = &indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset];
            const auto indexCount       = indexAccessor.count;
            const auto indexType        = indexAccessor.componentType;

            // Create mesh buffers
            Mesh* myMesh = new Mesh();
            myMesh->m_name = mesh.name;
            myMesh->m_indexType = indexType;
            myMesh->m_indexCount = indexCount;
            myMesh->m_material.Diffuse = Color(1, 1, 1, 1);

            CREATE_ZERO(D3D11_BUFFER_DESC, vertexBufferDesc);
            vertexBufferDesc.ByteWidth = sizeof(float) * positionCount * 3;
            vertexBufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexBufferData);
            vertexBufferData.pSysMem   = positionData;
            HR(D3D_DEVICE->CreateBuffer(&vertexBufferDesc, &vertexBufferData, myMesh->m_vertexBuffer.ReleaseAndGetAddressOf()));


            CREATE_ZERO(D3D11_BUFFER_DESC, indexBufferDesc);
            indexBufferDesc.ByteWidth = indexType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) * indexCount : sizeof(uint32_t) * indexCount;
            indexBufferDesc.Usage     = D3D11_USAGE_DEFAULT;
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexBufferData);
            indexBufferData.pSysMem   = indexData;
            HR(D3D_DEVICE->CreateBuffer(&indexBufferDesc, &indexBufferData, myMesh->m_indexBuffer.ReleaseAndGetAddressOf()));

            LOG("Loaded mesh [" << mesh.name << "] from gltf");
            m_meshes.push_back(myMesh);
        }
    }

    for (size_t i = 0; i < node.children.size(); i++)
    {
        assert((node.children[i] >= 0) && (node.children[i] < m_model.nodes.size()));
        ProcessNode(m_model.nodes[node.children[i]]);
    }
}