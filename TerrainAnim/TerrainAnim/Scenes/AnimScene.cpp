#include "pch.h"
#include "Scenes/AnimScene.h"

AnimScene::AnimScene(UINT width, UINT height)
	:
	Scene(width, height)
{
}

void AnimScene::Load()
{
	m_fbx = std::make_unique<FBX>();
}

void AnimScene::Update(float dt, const InputEvent& input)
{
}

void AnimScene::Render()
{
}

static const char* message = "No FBX loaded";
static bool loadFlag = true;
static bool meshFlag = true;
void AnimScene::GUI()
{
    static char buf[128] = "./Assets/Idle.fbx";
    
	if (ImGui::CollapsingHeader("Animation Scene Settings"))
	{

        ImGui::InputText("File", buf, 128, ImGuiInputTextFlags_CharsNoBlank);
    
        if (ImGui::Button("Load FBX"))
            loadFlag = m_fbx->LoadFBX(buf);
        
        if (!loadFlag)
        {
            ImGui::Text("Failed to load FBX. Check filename");
        }

        auto scene = m_fbx->GetScene();
        if (!scene)
        {
            ImGui::Text("No FBX scene loaded");
            return;
        }

        if (ImGui::Button("Generate Mesh"))
            meshFlag = m_fbx->GenerateMesh();

        if (!meshFlag)
        {
            ImGui::Text("No FBX mesh structure loaded");
        }


        ImGui::Spacing();

        const int meshCount = scene->getMeshCount();
        for (int i = 0; i < meshCount; ++i)
        {
            const ofbx::Mesh* mesh = scene->getMesh(i);
            ImGui::Text("Mesh: %s", mesh->name);
            if (mesh)
            {
                // Access the mesh data
                const int vertexCount      = mesh->getGeometry()->getVertexCount();
                const ofbx::Vec3* vertices = mesh->getGeometry()->getVertices();
                const ofbx::Vec3* normals  = mesh->getGeometry()->getNormals();
                const int indexCount       = mesh->getGeometry()->getIndexCount();
                const int* indices         = mesh->getGeometry()->getFaceIndices();

                // Process the mesh materials and textures
                const int material_count = mesh->getMaterialCount();
                for (int j = 0; j < material_count; ++j)
                {
                    const ofbx::Material* material = mesh->getMaterial(j);
                    if (material)
                    {
                        // Access the material data
                        const char* materialName     = material->name;
                        const ofbx::Texture* texture = material->getTexture(ofbx::Texture::DIFFUSE);
                        if (texture)
                        {
                            // Access the texture data
                            const auto texFilename = texture->getFileName();                        
                        }
                        ImGui::Text("Material: %s", materialName);
                    }
                }

                ImGui::Text("Vertex Count: %u", vertexCount);
                ImGui::Text("Index Count: %u", indexCount);

                ImGui::Separator();
            }
        }
	}
}

void AnimScene::Unload()
{
}
