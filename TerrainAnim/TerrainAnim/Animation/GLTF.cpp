#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#include "GLTF.h"
#include "Core/Timer.h"
#include "Graphics/Direct3D.h"
#include <assert.h>


#pragma region TinyGLTF Wrappers
struct GltfNodeWrapper
{
    const tinygltf::Node& Node;

    const auto& GetName()        const { return Node.name; }
    const auto& GetTranslation() const { return Node.translation; }
    const auto& GetRotation()    const { return Node.rotation; }
    const auto& GetScale()       const { return Node.scale; }
    const auto& GetMatrix()      const { return Node.matrix; }
    const auto& GetChildrenIds() const { return Node.children; }
    auto        GetMeshId()      const { return Node.mesh; }
    auto        GetSkinId()      const { return Node.skin; }
};

struct GltfPrimitiveWrapper
{
    const tinygltf::Primitive& Primitive;
    
    const int* GetAttribute(const char* name) const
    {
        auto iter = Primitive.attributes.find(name);
        return (iter != std::end(Primitive.attributes)) ? &iter->second : nullptr;
    }

    const auto& Get()    const { return Primitive; }
    auto GetIndicesId()  const { return Primitive.indices; }
    auto GetMaterialId() const { return Primitive.material; }
};

struct GltfMeshWrapper
{
    const tinygltf::Mesh& Mesh;

    const auto& Get()             const { return Mesh; }
    const auto& GetName()         const { return Mesh.name; }
    auto GetPrimitiveCount()      const { return Mesh.primitives.size(); }
    auto GetPrimitive(size_t Idx) const { return GltfPrimitiveWrapper{ Mesh.primitives[Idx] }; };
};

struct GltfBufferViewWrapper;
struct GltfAccessorWrapper
{
    const tinygltf::Accessor& Accessor;

    auto GetCount() const { return Accessor.count; }
    auto GetMinValues() const
    {
        return Vector3{
            static_cast<float>(Accessor.minValues[0]),
            static_cast<float>(Accessor.minValues[1]),
            static_cast<float>(Accessor.minValues[2]),
        };
    }

    auto GetMaxValues() const
    {
        return Vector3{
            static_cast<float>(Accessor.maxValues[0]),
            static_cast<float>(Accessor.maxValues[1]),
            static_cast<float>(Accessor.maxValues[2]),
        };
    }

    auto GetBufferViewId()  const { return Accessor.bufferView; }
    auto GetByteStride(const GltfBufferViewWrapper& view) const;
    auto GetByteOffset()    const { return Accessor.byteOffset; }
    auto GetComponentType() const { return Accessor.componentType; }
    auto GetNumComponents() const { return tinygltf::GetNumComponentsInType(Accessor.type); }
};

struct GltfBufferViewWrapper
{
    const tinygltf::BufferView& View;

    auto GetBufferId() const { return View.buffer; }
    auto GetByteOffset() const { return View.byteOffset; }
};

struct GltfBufferWrapper
{
    const tinygltf::Buffer& Buffer;

    const auto* GetData(size_t Offset) const { return &Buffer.data[Offset]; }
};

struct GltfSkinWrapper
{
    const tinygltf::Skin& Skin;

    const auto& GetName()                  const { return Skin.name; }
    auto        GetSkeletonId()            const { return Skin.skeleton; }
    auto        GetInverseBindMatricesId() const { return Skin.inverseBindMatrices; }
    const auto& GetJointIds()              const { return Skin.joints; }
};

struct GltfAnimationSamplerWrapper
{
    const tinygltf::AnimationSampler& Sam;

   /* AnimationSampler::InterpolationType GetInterpolation() const
    {
        if (Sam.interpolation == "LINEAR")
            return AnimationSampler::InterpolationType::Linear;
        else if (Sam.interpolation == "STEP")
            return AnimationSampler::InterpolationType::Step;
        else if (Sam.interpolation == "CUBICSPLINE")
            return AnimationSampler::InterpolationType::CubicSpline;
        else
        {
            LOG("Unexpected animation interpolation type: " << Sam.interpolation);
            return AnimationSampler::InterpolationType::Linear;
        }
    }*/

    auto GetInputId() const { return Sam.input; }
    auto GetOutputId() const { return Sam.output; }
};

struct GltfAnimationChannelWrapper
{
    const tinygltf::AnimationChannel& Channel;

    /*AnimationChannel::AnimPathType GetPathType() const
    {
        if (Channel.target_path == "rotation")
            return AnimationChannel::AnimPathType::Rotation;
        else if (Channel.target_path == "translation")
            return AnimationChannel::AnimPathType::Translation;
        else if (Channel.target_path == "scale")
            return AnimationChannel::AnimPathType::Scale;
        else if (Channel.target_path == "weights")
            return AnimationChannel::AnimPathType::Weights;
        else
        {
            LOG("Unsupported animation channel path " << Channel.target_path);
            return AnimationChannel::AnimPathType::Rotation;
        }
    }*/

    auto GetSamplerId() const { return Channel.sampler; }
    auto GetTargetNodeId() const { return Channel.target_node; }
};

struct GltfAnimationWrapper
{
    const tinygltf::Animation& Anim;

    const auto& GetName() const { return Anim.name; }

    auto GetSamplerCount() const { return Anim.samplers.size(); }
    auto GetChannelCount() const { return Anim.channels.size(); }
    auto GetSampler(size_t Id) const { return GltfAnimationSamplerWrapper{ Anim.samplers[Id] }; }
    auto GetChannel(size_t Id) const { return GltfAnimationChannelWrapper{ Anim.channels[Id] }; }
};

struct GltfModelWrapper
{
    const tinygltf::Model& Model;

    auto GetNode(int idx)         const { return GltfNodeWrapper{ Model.nodes[idx] }; }
    auto GetMesh(int idx)         const { return GltfMeshWrapper{ Model.meshes[idx] }; }
    auto GetAccessor(int idx)     const { return GltfAccessorWrapper{ Model.accessors[idx] }; }
    auto GetBufferView(int idx)   const { return GltfBufferViewWrapper{ Model.bufferViews[idx] }; }
    auto GetBuffer(int idx)       const { return GltfBufferWrapper{ Model.buffers[idx] }; }
    auto GetSkinCount()           const { return Model.skins.size(); }
    auto GetSkin(size_t idx)      const { return GltfSkinWrapper{ Model.skins[idx] }; }
    auto GetAnimationCount()      const { return Model.animations.size(); }
    auto GetAnimation(size_t idx) const { return GltfAnimationWrapper{ Model.animations[idx] }; }
};

auto GltfAccessorWrapper::GetByteStride(const GltfBufferViewWrapper& View) const { return Accessor.ByteStride(View.View); }
#pragma endregion


#pragma region GLTF Helpers
Matrix GetInvBindMatrix(const tinygltf::Model& model, const int matrixId)
{
    GltfAccessorWrapper accessor{ model.accessors[matrixId] };
    GltfBufferViewWrapper bufferView{ model.bufferViews[accessor.GetBufferViewId()] };
    GltfBufferWrapper buffer{ model.buffers[bufferView.GetBufferId()] };

    const float* data = reinterpret_cast<const float*>(buffer.GetData(bufferView.GetByteOffset() + accessor.GetByteOffset()));

    return Matrix(data);
}

Matrix GetTransformationMatrix(GltfNodeWrapper& node)
{
    const auto& posVector = node.GetTranslation();
    const auto& rotVector = node.GetRotation();
    const auto& sclVector = node.GetScale();
    
    Vector3 position    = Vector3::Zero;
    Quaternion rotation = Quaternion::Identity;
    Vector3 scale       = Vector3::One;

    if (posVector.size() == 3)
        position = Vector3((float)posVector[0], (float)posVector[1], (float)posVector[2]);
    if (rotVector.size() == 4)
        rotation = Quaternion((float)rotVector[0], (float)rotVector[1], (float)rotVector[2], (float)rotVector[3]);
    if (sclVector.size() == 3)
        scale = Vector3((float)sclVector[0], (float)sclVector[1], (float)sclVector[2]);

    Matrix transform = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
    return transform;
}

Joint* BuildJointTree(const tinygltf::Model model, const GltfSkinWrapper& skin, const std::vector<int>& jointIds, int rootJointId, Joint* parent)
{
    GltfNodeWrapper jointNode{ model.nodes[rootJointId] };

    Joint* rootJoint = new Joint;
    rootJoint->Parent = parent;
    rootJoint->Index = rootJointId;
    rootJoint->Name = jointNode.GetName();
    rootJoint->InverseBindMatrix = GetInvBindMatrix(model, skin.GetInverseBindMatricesId());
    rootJoint->Transform = GetTransformationMatrix(jointNode);


    std::vector<Joint*> childJoints;
    for (const auto& childJointId : jointNode.GetChildrenIds())
    {
        GltfNodeWrapper childJointNode{ model.nodes[childJointId] };

        Joint* child = BuildJointTree(model, skin, skin.GetJointIds(), childJointId, rootJoint);
        childJoints.push_back(child);
    }
    rootJoint->Children = childJoints;
    return rootJoint;
}
#pragma endregion




GLTF::GLTF()
    :
    m_scaleFactor(100.0f)
{
}

GLTF::~GLTF()
{
}

bool GLTF::Load(const char* filename)
{
    Timer loadTime;
    loadTime.Start();
    tinygltf::TinyGLTF loader;

    std::string err, warn;
    tinygltf::Model model;
    if (!loader.LoadASCIIFromFile(&model, &err, &warn, filename))
    {
        // Failed to load ASCII, try binary
        if (!loader.LoadBinaryFromFile(&model, &err, &warn, filename))
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
    if (model.scenes.size() > 1)
    {
        LOG("Parsing GLTF files with multipe scenes not supported!");
        return false;
    }
    

    ProcessModel(model);

    // Generate the D3D vertex and index buffers
    for (auto& mesh : m_meshes)
    {
        mesh->GeneratePrimitiveBuffers();
    }
    

    loadTime.Stop();
    LOG("Loaded file [" << filename << "] in " << loadTime.TotalTime() * 1000.0f << "ms");
    LOG("\n---------- END GLTF FILE PARSING ----------\n")
    return true;
}

void GLTF::ProcessModel(const tinygltf::Model& model)
{
    // Loop through all the nodes and get nodes with meshes
    std::vector<int> meshNodes;
    for (int i = 0; i < model.nodes.size(); i++)
    {
        if (model.nodes[i].mesh != -1)
            meshNodes.push_back(i);
    }

    for (const auto& meshNodeIndex : meshNodes)
    {
        GltfNodeWrapper meshNode { model.nodes[meshNodeIndex] };
        GltfMeshWrapper mesh { model.meshes[meshNode.GetMeshId()]};

        Mesh* myMesh = new Mesh;
        myMesh->Name = mesh.GetName();

        // Loop through primitives
        for (int i = 0; i < mesh.GetPrimitiveCount(); i++)
        {
            GltfPrimitiveWrapper primitive{ mesh.GetPrimitive(i) };
            Primitive myPrimitive;

            // Set parent mesh
            myPrimitive.ParentMesh = myMesh;

            // Get primitive material data
            {
                const auto& gltfMaterial = model.materials[primitive.GetMaterialId()];
                const auto& pbr          = gltfMaterial.pbrMetallicRoughness;
                const auto& baseColor    = pbr.baseColorFactor;  // Diffuse color

                // Set diffuse color
                myPrimitive.DiffuseColor = Color((float)baseColor[0], (float)baseColor[1], (float)baseColor[2], (float)baseColor[3]);
            }

            // Get vertex attributes
            {
                GltfAccessorWrapper positionAccessor { model.accessors[*primitive.GetAttribute("POSITION")] };    // TODO: Add nullptr check
                GltfAccessorWrapper normalAccessor   { model.accessors[*primitive.GetAttribute("NORMAL")] };      // TODO: Add nullptr check
                GltfAccessorWrapper texCoordAccessor { model.accessors[*primitive.GetAttribute("TEXCOORD_0")] };  // TODO: Add nullptr check
                

                GltfBufferViewWrapper positionBufferView { model.bufferViews[positionAccessor.GetBufferViewId()] };
                GltfBufferViewWrapper normalBufferView   { model.bufferViews[normalAccessor.GetBufferViewId()] };
                GltfBufferViewWrapper texCoordBufferView { model.bufferViews[texCoordAccessor.GetBufferViewId()] };

                GltfBufferWrapper positionBuffer { model.buffers[positionBufferView.GetBufferId()] };
                GltfBufferWrapper normalBuffer   { model.buffers[normalBufferView.GetBufferId()] };
                GltfBufferWrapper texCoordBuffer { model.buffers[texCoordBufferView.GetBufferId()] };


                const float* positionData       = reinterpret_cast<const float*>(positionBuffer.GetData(positionBufferView.GetByteOffset() + positionAccessor.GetByteOffset()));
                const float* normalData         = reinterpret_cast<const float*>(normalBuffer.GetData(normalBufferView.GetByteOffset()     + normalAccessor.GetByteOffset()));
                const float* texCoordData       = reinterpret_cast<const float*>(texCoordBuffer.GetData(texCoordBufferView.GetByteOffset() + texCoordAccessor.GetByteOffset()));

                // Apply data to my primitive
                const int verticesCount = static_cast<int>(positionAccessor.GetCount());
                for (int i = 0; i < verticesCount; i++)
                {
                    SimpleVertex vertex;

                    vertex.Pos      = Vector3(positionData[0], positionData[1], positionData[2]) * m_scaleFactor;
                    vertex.Normal   = Vector3(normalData[0], normalData[1], normalData[2]);
                    vertex.TexCoord = Vector2(texCoordData[0], texCoordData[1]);

                    // Move forward in vertex attributes buffers
                    positionData += 3;  // X, Y, Z
                    normalData   += 3;  // X, Y, Z
                    texCoordData += 2;  // X, Y

                    myPrimitive.Vertices.push_back(vertex);
                    //LOG("Position [" << LOG_VEC3(vertex.Pos) << "]\tNormal [" << LOG_VEC3(vertex.Normal) << "]\tUV [" << LOG_VEC2(vertex.TexCoord) << "]");
                }

                
                auto jointAttribute  = primitive.GetAttribute("JOINTS_0");
                auto weightAttribute = primitive.GetAttribute("WEIGHTS_0");
                if (jointAttribute != nullptr && weightAttribute != nullptr)
                {
                    // Get joint and weight information
                    GltfAccessorWrapper jointAccessor     { model.accessors[*jointAttribute] };
                    GltfAccessorWrapper weightAccessor    { model.accessors[*weightAttribute] };
                    GltfBufferViewWrapper jointBufferView { model.bufferViews[jointAccessor.GetBufferViewId()] };
                    GltfBufferViewWrapper weightBufferView{ model.bufferViews[weightAccessor.GetBufferViewId()] };
                    GltfBufferWrapper jointBuffer         { model.buffers[jointBufferView.GetBufferId()] };
                    GltfBufferWrapper weightBuffer        { model.buffers[weightBufferView.GetBufferId()] };

                    const BYTE* jointData  = reinterpret_cast<const BYTE*>(jointBuffer.GetData(jointBufferView.GetByteOffset()   + jointAccessor.GetByteOffset()));
                    const float* weightData = reinterpret_cast<const float*>(weightBuffer.GetData(weightBufferView.GetByteOffset() + weightAccessor.GetByteOffset()));
                    
                    const auto jointType = jointAccessor.GetComponentType();
                    const auto weightType = weightAccessor.GetComponentType();

                    
                    const int jointsCount = static_cast<int>(jointAccessor.GetCount());
                    for (int i = 0; i < jointsCount; i++)
                    {
                        Vector4 joint  = Vector4(jointData[0], jointData[1], jointData[2], jointData[3]);
                        Vector4 weight = Vector4(weightData[0], weightData[1], weightData[2], weightData[3]);

                        myPrimitive.Joints.push_back(joint);
                        myPrimitive.Weights.push_back(weight);

                        jointData  += 4;
                        weightData += 4;

                        //LOG("Joint [" << LOG_VEC4(joint) << "] --- Weight [" << LOG_VEC4(weight) << "]");
                    }
                }
            }


            // Get the index buffer
            if (primitive.GetIndicesId() > -1)
            {
                GltfAccessorWrapper indexAccessor     { model.accessors[primitive.GetIndicesId()] };
                GltfBufferViewWrapper indexBufferView { model.bufferViews[indexAccessor.GetBufferViewId()] };
                GltfBufferWrapper indexBuffer         { model.buffers[indexBufferView.GetBufferId()] };
                
                const int* indexData = reinterpret_cast<const int*>(indexBuffer.GetData(indexBufferView.GetByteOffset() + indexAccessor.GetByteOffset()));
                
                const int indicesCount = (int)indexAccessor.GetCount();

                for (int idx = 0; idx < indicesCount; idx++)
                {
                    int index = indexData[idx];

                    // Index may be negative, this implies that the offset is from the end of the vertex buffer
                    if (index < 0)
                        index = static_cast<int>(myPrimitive.Vertices.size());

                    myPrimitive.Indices.push_back(index);

                    //LOG("Index: " << idx << " [" << index << "]");
                }
            }
            myMesh->Primitives.push_back(myPrimitive);
        }

        if (meshNode.GetSkinId() == -1)
        {
            m_meshes.push_back(myMesh);
            continue;
        }
        

        // ----- Extract Skininng Data -----

        GltfSkinWrapper skin{ model.skins[meshNode.GetSkinId()] };
        myMesh->LinkedSkin.Name = skin.GetName();
        
        // Loop through all the joints and create a parent child map
        const auto& jointIdList = skin.GetJointIds();
        
        // NOTE: This recursive function takes a lot of time
        myMesh->LinkedSkin.JointTree = BuildJointTree(model, skin, jointIdList, jointIdList[0], nullptr);


        m_meshes.push_back(myMesh);
    }
    meshNodes.clear();
}

// https://github.com/supernovaengine/supernova/blob/master/engine/core/object/Model.h
// https://github.com/DiligentGraphics/DiligentTools/blob/master/AssetLoader/src/GLTFBuilder.cpp
// https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.h
// https://github.com/supernovaengine/supernova/blob/master/engine/core/subsystem/MeshSystem.cpp#L1448