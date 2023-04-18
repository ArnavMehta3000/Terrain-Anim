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
    ProcessJoints();
    ProcessAnimation();

    for (auto& joint : m_joints)
    {
        auto mat = joint.InvBindTransform;
        joint.CalculateInverseBindTransform(Matrix::Identity);
        auto& local = joint.LocalBindTransform;
        int x = 0;
    }

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

void GLTF::ProcessJoints()
{
    // Parent index - children indices
    std::unordered_map<int, std::vector<int>> parentMap;
    for (auto& skin : m_model.skins)
    {
        for (auto& jointIndex : skin.joints)
        {
            const auto& jointNode = m_model.nodes[jointIndex];

            // Loop over all children of current joint
            for (auto& child : jointNode.children)
            {
                parentMap[jointIndex].push_back(child);
            }
        }
    }
    
    for (auto& pair : parentMap)
    {        
        const auto& rootNode = m_model.nodes[pair.first];

        Joint parentJoint{};
        parentJoint.Name   = rootNode.name;
        parentJoint.NodeId = pair.first;

        for (auto& child : pair.second)
        {
            const auto& childNode = m_model.nodes[child];

            Joint childJoint{};
            childJoint.Name   = childNode.name;
            childJoint.NodeId = child;
            parentJoint.Children.push_back(childJoint);
        }
        m_joints.push_back(parentJoint);
    }

    for (auto& joint : m_joints)
    {
        joint.Print();
    }
}

void GLTF::ProcessAnimation()
{
    LOG("\n\n");
    for (auto& skin : m_model.skins)
    {
        for (auto& joint : m_joints)
        {
            const int jointIndex = joint.NodeId;
            const auto& joints = skin.joints;

            if (!(jointIndex >= 0 && jointIndex < (int)joints.size()))
                continue;
            
            const auto& jointNode = m_model.nodes[jointIndex];
            
            // This joint is linked with this skin
            //LOG("Joint skin found: " << jointIndex << "   Name: " << m_model.nodes[jointIndex].name);
            

            const auto& accessor           = m_model.accessors[skin.inverseBindMatrices];
            const auto& bufferView         = m_model.bufferViews[accessor.bufferView];
            const auto& buffer             = m_model.buffers[bufferView.buffer];
            const uint8_t* data            = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
            const uint32_t stride          = accessor.ByteStride(bufferView);
            const float* inverseBindMatrix = reinterpret_cast<const float*>(data + jointIndex * stride);
            
            XMFLOAT4X4 inverseBindMatrixXM;
            for (int row = 0; row < 4; ++row) 
            {
                for (int col = 0; col < 4; ++col) 
                {
                    inverseBindMatrixXM.m[row][col] = inverseBindMatrix[row * 4 + col];
                }
            }

            const auto& translation = Vector3((float)jointNode.translation[0], (float)jointNode.translation[1], (float)jointNode.translation[2]);
            const auto& rotation    = Quaternion((float)jointNode.rotation[0], (float)jointNode.rotation[1], (float)jointNode.rotation[2], (float)jointNode.rotation[3]);
            const auto& scale       = (jointNode.scale.size() != 0) ? Vector3((float)jointNode.scale[0], (float)jointNode.scale[1], (float)jointNode.scale[2]) : Vector3(1.0f, 1.0f, 1.0f);

            joint.LocalBindTransform = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);
            joint.InvBindTransform = inverseBindMatrixXM;
        }
    }
}
