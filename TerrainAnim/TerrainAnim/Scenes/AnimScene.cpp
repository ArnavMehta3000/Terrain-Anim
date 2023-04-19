#include "pch.h"
#include "Scenes/AnimScene.h"
#include "Graphics/Direct3D.h"

AnimScene::AnimScene(UINT width, UINT height)
	:
	Scene(width, height)
{
}

void AnimScene::Load()
{
    m_sceneCamera.Position(Vector3(20.0f, 85.0f, -140.0f));
	m_gltf   = std::make_unique<GLTF>();
    m_shader = std::make_unique<Shader>(L"Shaders/Anim/Anim_VS.hlsl", L"Shaders/Anim/Anim_PS.hlsl");
    D3D->CreateConstantBuffer(m_materialCBuffer, sizeof(Material));
    
    if (m_wvpBuffer == nullptr)
        D3D->CreateConstantBuffer(m_wvpBuffer, sizeof(WVPBuffer));
}

void AnimScene::Update(float dt, const InputEvent& input)
{
    m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

    WVPBuffer wvp
    {
        .World      = Matrix::Identity.Transpose(),
        .View       = m_sceneCamera.GetView().Transpose(),
        .Projection = m_sceneCamera.GetProjection().Transpose()
    };

    D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void AnimScene::Render()
{
    m_shader->BindPS();
    m_shader->BindVS(true);

    D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());
    D3D_CONTEXT->PSSetConstantBuffers(0, 1, m_materialCBuffer.GetAddressOf());
    

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    for (auto& mesh : m_gltf->GetMeshes())
    {
        for (auto& primitive : mesh->Primitives)
        {
            // Set primitive material
            Material mat{ .Diffuse = primitive.DiffuseColor };
            D3D_CONTEXT->UpdateSubresource(m_materialCBuffer.Get(), 0, nullptr, &mat, 0, 0);



            D3D_CONTEXT->IASetVertexBuffers(0, 1, primitive.m_vertexBuffer.GetAddressOf(), &stride, &offset);
            D3D_CONTEXT->IASetIndexBuffer(primitive.m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

            D3D_CONTEXT->DrawIndexed((UINT)primitive.Indices.size(), 0, 0);
        }
    }
}

void AnimScene::Unload()
{
}

static const char* message = "No GLTF loaded";
static bool loadFlag = true;
static bool meshFlag = true;
void AnimScene::GUI()
{
    
	if (ImGui::CollapsingHeader("Animation Scene Settings"))
	{

        DrawFBXInfo();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        DrawMeshInfo();        
	}
}

void AnimScene::DrawFBXInfo()
{
    if (ImGui::TreeNodeEx("GLTF Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static char buf[128] = "./Assets/Idle.gltf";

        ImGui::InputText("File", buf, 128, ImGuiInputTextFlags_CharsNoBlank);

        if (ImGui::Button("Load GLTF"))
            loadFlag = m_gltf->Load(buf);

        if (!loadFlag)
        {
            ImGui::Text("Failed to load GLTF. Check filename");

        }

        if (!meshFlag)
        {
            ImGui::Text("No GLTF mesh structure loaded");
        }
        ImGui::TreePop();
    }
}


static float scaleFactor = 1.0f;
static Joint* selectedJoint = nullptr;


void DrawJointData()
{
    if (selectedJoint == nullptr)
        return;

    if (ImGui::TreeNodeEx((std::string("Joint Data: ").append(selectedJoint->Name)).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Joint Index: %u", selectedJoint->Index);
        ImGui::Text("Joint Child Count: %u", selectedJoint->Children.size());
        
        ImGui::TreePop();
    }
}

void DrawSkeleton(Joint* joint, int level = 0)
{
    //LOG(std::string(level * 2, ' ') << " - Joint: " << joint->Name << " (id: " << joint->Index << ")");
    if (joint->Children.size() == 0)
    {
        if (ImGui::TreeNodeEx(joint->Name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
        {
            selectedJoint = joint;
            ImGui::TreePop();
        }
    }
    else
    {
        bool node_open = ImGui::TreeNodeEx(joint->Name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
        if (ImGui::IsItemClicked())
        {
            selectedJoint = joint;
        }
        if (node_open)
        {
            // Recursively draw joint data
            for (auto& childJoint : joint->Children)
                DrawSkeleton(childJoint.get(), level + 1);

            ImGui::TreePop();
        }
    }
}

void AnimScene::DrawMeshInfo()
{
    if (ImGui::CollapsingHeader("Mesh Map", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        for (auto& mesh : m_gltf->GetMeshes())
        {
            if (ImGui::CollapsingHeader((std::string("Skeleton - ").append(mesh->Name)).c_str()))
            {
                DrawJointData();
                ImGui::Separator();

                DrawSkeleton(mesh->LinkedSkin.JointTree.get());
            }
            ImGui::Spacing();
            ImGui::Spacing();
        }
    }
}
