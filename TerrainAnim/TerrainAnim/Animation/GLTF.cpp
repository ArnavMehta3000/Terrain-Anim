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
    auto GetByteOffset()    const { return Accessor.byteOffset; }
    auto GetComponentType() const { return Accessor.componentType; }
    auto GetNumComponents() const { return tinygltf::GetNumComponentsInType(Accessor.type); }
    auto GetByteStride(const GltfBufferViewWrapper& view) const;
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

    AnimationSampler::InterpolationType GetInterpolation() const
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
    }

    auto GetInputId() const { return Sam.input; }
    auto GetOutputId() const { return Sam.output; }
};

struct GltfAnimationChannelWrapper
{
    const tinygltf::AnimationChannel& Channel;

    AnimationChannel::AnimPathType GetPathType() const
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
    }

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




#pragma region Helper Functions
Matrix GetNodeMatrix(int nodeIndex, const tinygltf::Model& model)
{
    GltfNodeWrapper node{ model.nodes[nodeIndex] };
    
    const auto& posVector = node.GetTranslation();
    const auto& rotVector = node.GetRotation();
    const auto& scaleVector = node.GetScale();


    return Matrix::Identity;
}

Matrix GetGlobalMatrix(int nodeIndex, const tinygltf::Model& model, std::map<int, int>& nodesParent)
{
    Matrix mat = GetNodeMatrix(nodeIndex, model);
    return Matrix::Identity;
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

    

    loadTime.Stop();
    LOG("Loaded file [" << filename << "] in " << loadTime.TotalTime() * 1000.0f << "ms");
    LOG("\n---------- END GLTF FILE PARSING ----------\n")
    return true;
}

void GLTF::ProcessModel(const tinygltf::Model& model)
{
    int meshNode = -1;
    int meshIndex = 0;
    std::map<int, int> nodesParent;

    for (int i = 0; i < model.nodes.size(); i++)
        nodesParent[i] = -1;

    for (int i = 0; i < model.nodes.size(); i++)
    {
        GltfNodeWrapper node { model.nodes[i] };
        
        if (node.GetMeshId() == meshIndex)
            meshNode = i;

        const auto& children = node.GetChildrenIds();
        for (int c = 0; c < children.size(); c++)
            nodesParent[children[c]] = i;
    }

    Matrix globalMat = GetGlobalMatrix(meshNode, model, nodesParent);
}

// https://github.com/supernovaengine/supernova/blob/master/engine/core/object/Model.h
// https://github.com/DiligentGraphics/DiligentTools/blob/master/AssetLoader/src/GLTFBuilder.cpp
// https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.h

// https://github.com/supernovaengine/supernova/blob/master/engine/core/subsystem/MeshSystem.cpp#L1448