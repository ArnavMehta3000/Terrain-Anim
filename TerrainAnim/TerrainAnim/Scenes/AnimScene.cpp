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

	m_fbx = std::make_unique<FBX>();
}

void AnimScene::Update(float dt, const InputEvent& input)
{
    m_sceneCamera.Update(dt, input.KeyboardState, input.MouseState);

    WVPBuffer wvp
    {
        .View = m_sceneCamera.GetView().Transpose(),
        .Projection = m_sceneCamera.GetProjection().Transpose()
    };

    std::for_each(m_fbx->GetMeshList().cbegin(), m_fbx->GetMeshList().cend(),
        [&, dt, input](const std::unique_ptr<Mesh>& mesh)
        {
            mesh->Update(dt, input);
            wvp.World = mesh->GetWorldMatrix().Transpose();
        });

    D3D_CONTEXT->UpdateSubresource(m_wvpBuffer.Get(), 0, nullptr, &wvp, 0, 0);
}

void AnimScene::Render()
{
    D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_wvpBuffer.GetAddressOf());
    
    std::for_each(m_fbx->GetMeshList().cbegin(), m_fbx->GetMeshList().cend(),
        [](const std::unique_ptr<Mesh>& mesh)
        {
            mesh->Render();
        });

}

void AnimScene::Unload()
{
}

static const char* message = "No FBX loaded";
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
    if (ImGui::TreeNodeEx("FBX Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static char buf[128] = "./Assets/Idle.fbx";

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
            ImGui::TreePop();
            return;
        }

        if (ImGui::Button("Generate Mesh"))
            meshFlag = m_fbx->GenerateMesh();

        if (!meshFlag)
        {
            ImGui::Text("No FBX mesh structure loaded");
        }

        ImGui::TreePop();
    }
}


static float scaleFactor = 1.0f;
void AnimScene::DrawMeshInfo()
{
    if (ImGui::TreeNodeEx("Mesh Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (m_fbx->GetMeshList().size() == 0)
            ImGui::TextColored({ 1, 1, 0, 1 }, "Mesh Not extracted from FBX");

        std::for_each(m_fbx->GetMeshList().cbegin(), m_fbx->GetMeshList().cend(),
            [](const std::unique_ptr<Mesh>& mesh)
            {
                mesh->GUI();
            });

        ImGui::TreePop();
     }
}
