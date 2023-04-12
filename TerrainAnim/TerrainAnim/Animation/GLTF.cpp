#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#include "GLTF.h"
#include "Animation/Animation.h"
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
    ProcessAnimations();
    ProcessJoints();

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

void GLTF::ProcessAnimations()
{
    const auto& animations = m_model.animations;
    
    LOG("Animation Count: " << animations.size());

    for (const auto& anim : animations)
    {
        Animation myAnim{};
        myAnim.Name = anim.name;

        for (const auto& channel : anim.channels)
        {
            AnimationSampler mySampler{};

            // Node index that the animation is targetting
            const int nodeIndex = channel.target_node;

            // Animation sampler contains the keyframe data
            const int samplerIndex = channel.sampler;
            const auto& sampler = anim.samplers[samplerIndex];

            
            if (sampler.interpolation == "LINEAR")
                mySampler.Interpolation = AnimationSampler::InterpolationType::LINEAR;
            if (sampler.interpolation == "STEP")
                mySampler.Interpolation = AnimationSampler::InterpolationType::STEP;
            if (sampler.interpolation == "CUBICSPLINE")
                mySampler.Interpolation = AnimationSampler::InterpolationType::CUBICSPLINE;



            // Input and output accessors contain the time and value data of the animation
            const int inputIndex       = sampler.input;
            const int outputIndex      = sampler.output;
            const auto& inputAccessor  = m_model.accessors[inputIndex];
            const auto& outputAccessor = m_model.accessors[outputIndex];

            // Get buffer views and buffer that contain the actual keyframe data
            const int inputBufferViewIndex  = inputAccessor.bufferView;
            const int outputBufferViewIndex = outputAccessor.bufferView;
            const auto& inputBufferView     = m_model.bufferViews[inputBufferViewIndex];
            const auto& outputBufferView    = m_model.bufferViews[outputBufferViewIndex];
            const auto& inputBuffer         = m_model.buffers[inputBufferView.buffer];
            const auto& outputBuffer        = m_model.buffers[outputBufferView.buffer];

            // Get type and component type of the keyframe data
            const int inputType           = inputAccessor.type;
            const int outputType          = outputAccessor.type;
            const int inputComponentType  = inputAccessor.componentType;
            const int outputComponentType = outputAccessor.componentType;

            const float* inputBufferPtr = reinterpret_cast<const float*>(&inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset]);
            const void* outputBufferPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];

            // Read sampler inputs (time)
            for (size_t i = 0; i < inputAccessor.count; i++)
                mySampler.Inputs.push_back(inputBufferPtr[i]);

            // Read sampler values (value)
            switch (outputAccessor.type)
            {
            case TINYGLTF_TYPE_VEC3:
            {
                const Vector3* buffer = static_cast<const Vector3*>(outputBufferPtr);
                for (size_t i = 0; i < outputAccessor.count; i++)
                {
                    auto v3 = buffer[i];
                    mySampler.Outputs.push_back(Vector4(v3.x, v3.y, v3.z, 0.0f));
                }
            }
            break;

            case TINYGLTF_TYPE_VEC4:
            {
                const Vector4* buffer = static_cast<const Vector4*>(outputBufferPtr);
                for (size_t i = 0; i < outputAccessor.count; i++)
                    mySampler.Outputs.push_back(buffer[i]);
            }
            break;
            }

            for (auto& input : mySampler.Inputs)
            {
                if (input < myAnim.Start)
                    myAnim.Start = input;
                if (input > myAnim.End)
                    myAnim.End = input;
            }

            // Read channel data
            AnimationChannel myChannel{};
            myChannel.SamplerIndex = samplerIndex;

            if (channel.target_path == "rotation")
                myChannel.Path = AnimationChannel::PathType::ROTATION;
            if (channel.target_path == "translation")
                myChannel.Path = AnimationChannel::PathType::TRANSLATION;
            if (channel.target_path == "scale")
                myChannel.Path = AnimationChannel::PathType::SCALE;
            if (channel.target_path == "weights")
                myChannel.Path = AnimationChannel::PathType::WEIGHT;

            myAnim.Samplers.push_back(mySampler);
        }
        m_animations.push_back(myAnim);
    }
}

void GLTF::ProcessJoints()
{
    
}
