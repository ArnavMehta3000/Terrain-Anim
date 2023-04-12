#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#include "GLTF.h"
#include "Core/Timer.h"
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
    LOG("\n---------- BEGIN GLTF FILE PARSING ----------\n")
    if (m_model.scenes.size() > 1)
    {
        LOG("Parsing GLTF files with multipe scenes not supported!");
        return false;
    }
    
    ProcessMesh();
    ProcessSkeleton();
    ProcessAnimation();

    loadTime.Stop();
    LOG("Loaded file [" << filename << "] in " << loadTime.TotalTime() * 1000.0f << "ms");
    LOG("\n---------- END GLTF FILE PARSING ----------\n")
    return true;
}

void GLTF::ProcessMesh()
{
    // Loop over all the meshes in the model
    for (auto& mesh : m_model.meshes)
    {
        LOG("Mesh [" << mesh.name << "] contains " << mesh.primitives.size() << " primitives");
        Mesh* myMesh = new Mesh();
        myMesh->m_name = mesh.name;
        //myMesh->m_material.Name;
        
        // Loop over all the primitves in the mesh
        for (auto& primitive : mesh.primitives)
        {
            // Get primitive material data and apply to mesh
            const auto& materialIndex = primitive.material;
            const auto& gltfMaterial  = m_model.materials[materialIndex];
            const auto& pbr           = gltfMaterial.pbrMetallicRoughness;
            const auto& baseColor     = pbr.baseColorFactor;  // Diffuse color

            // Apply gathered data to my mesh
            //myMesh->m_material.Name    = gltfMaterial.name;
            myMesh->m_material.Diffuse = Color((float)baseColor[0], (float)baseColor[1], (float)baseColor[2], (float)baseColor[3]);

            // Get vertex attributes
            const auto& positionAccessor = m_model.accessors[primitive.attributes["POSITION"]];
            const auto& normalAccessor   = m_model.accessors[primitive.attributes["NORMAL"]];
            const auto& uvAccessor       = m_model.accessors[primitive.attributes["TEXCOORD_0"]];

            // Get the buffer view objects for each attribute
            const auto& positionBufferView = m_model.bufferViews[positionAccessor.bufferView];
            const auto& normalBufferView   = m_model.bufferViews[normalAccessor.bufferView];
            const auto& uvBufferView       = m_model.bufferViews[uvAccessor.bufferView];

            // Get the buffer data for each attribute
            const float* positionData = reinterpret_cast<const float*>(&m_model.buffers[positionBufferView.buffer].data[positionBufferView.byteOffset + positionAccessor.byteOffset]);
            const float* normalData   = reinterpret_cast<const float*>(&m_model.buffers[normalBufferView.buffer].data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
            const float* uvData       = reinterpret_cast<const float*>(&m_model.buffers[uvBufferView.buffer].data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
        
           
            // Convert to my vertex structure
            const int verticesCount = static_cast<int>(positionAccessor.count);
            for (int i = 0; i < verticesCount; i++)
            {
                SimpleVertex vertex;

                vertex.Pos      = Vector3(positionData[0], positionData[1], positionData[2]) * m_scaleFactor;
                vertex.Normal   = Vector3(normalData[0], normalData[1], normalData[2]);
                vertex.TexCoord = Vector2(uvData[0], uvData[1]);
                
                // Move forward in vertex attributes buffers
                positionData += 3;  // X, Y, Z
                normalData   += 3;  // X, Y, Z
                uvData       += 2;  // X, Y

                myMesh->m_vertices.push_back(vertex);
                //LOG("Position [" << LOG_VEC3(vertex.Pos) << "]\tNormal [" << LOG_VEC3(vertex.Normal) << "]\tUV [" << LOG_VEC2(vertex.TexCoord) << "]");
            }
        

            // Get the index buffer from model and save indices
            if (primitive.indices >= 0)
            {
                // Get index buffer accessor
                const auto& indexAccessor = m_model.accessors[primitive.indices];

                // Get view to index buffer
                const auto& indexBufferView = m_model.bufferViews[indexAccessor.bufferView];

                // Get the indices data from the buffer
                const auto indexData = reinterpret_cast<const int*>(&m_model.buffers[indexBufferView.buffer].data[indexBufferView.byteOffset + indexAccessor.byteOffset]);

                const auto indicesCount = static_cast<int>(indexAccessor.count);
                myMesh->m_indexCount = indicesCount;
                for (int i = 0; i < indicesCount; i++)
                {
                    int index = indexData[i];
                    
                    // Index may be negative, this implies that the offset is from the end of the vertex buffer
                    if (index < 0)
                        index = static_cast<int>(myMesh->m_vertices.size());

                    myMesh->m_indices.push_back(index);

                    //LOG("Index: " << i << "[" << index << "]");
                }
            }
        }

        myMesh->GenBuffers();
        m_meshes.push_back(myMesh);

        LOG("\n");
    }
}

void GLTF::ProcessSkeleton()
{
    // Parent index - children indices
    GetJointParentMap(m_parentMap);    

    
    for (auto& pair : m_parentMap)
    {        
        const auto& rootNode = m_model.nodes[pair.first];

        Joint* parentJoint  = new Joint;
        parentJoint->Name   = rootNode.name;
        parentJoint->NodeId = pair.first;
        parentJoint->Parent = nullptr;


        //LOG("Root: " << rootNode.name);

        for (auto& child : pair.second)
        {
            const auto& childNode = m_model.nodes[child];
            auto& mat = childNode.tr;

            Joint* childJoint = new Joint;
            childJoint->Name = childNode.name;
            childJoint->NodeId = child;
            childJoint->Parent = parentJoint;

            m_joints.push_back(childJoint);

            //LOG("\tChild: " << childNode.name);
        }
    }

    for (auto& joint : m_joints)
    {
        joint->Print();
    }
}

void GLTF::ProcessAnimation()
{
}

void GLTF::GetJointParentMap(std::unordered_map<int, std::vector<int>>& parentMap)
{
    std::vector<int> rootJoints;
    for (auto& skin : m_model.skins)
    {
        for (auto& jointIndex : skin.joints)
        {
            const auto& jointNode = m_model.nodes[jointIndex];

            //LOG("Joint: " << jointNode.name);

            if (jointNode.children.size() != 0)
                rootJoints.push_back(jointIndex);

            for (auto& child : jointNode.children)
            {
                //LOG("\tChild: " << m_model.nodes[child].name);
            }
            //LOG("\n")
        }
    }

    for (auto& rootIndex : rootJoints)
    {
        const auto& node = m_model.nodes[rootIndex];
        for (auto& child : node.children)
        {
            parentMap[rootIndex].push_back(child);
        }
    }
}
