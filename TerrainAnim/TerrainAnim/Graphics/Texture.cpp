#include "pch.h"
#include "Graphics/Texture.h"
#include "Graphics/Direct3D.h"

Texture2D::Texture2D(const wchar_t* fileName)
	:
	m_srv(nullptr)
{
    if (IsPathEmpty(fileName))
    {
        LOG("--- Texture2D file path empty ---");
        return;
    }

    ID3D11Resource* res = nullptr;
    DirectX::CreateWICTextureFromFile(D3D_DEVICE,
        fileName, &res, nullptr);

    HR(res->QueryInterface(IID_ID3D11Texture2D, (void**)m_texture.ReleaseAndGetAddressOf()));
    
    m_texture->GetDesc(&m_desc);

    HR(DirectX::CreateWICTextureFromFileEx(D3D_DEVICE, D3D_CONTEXT,
        fileName,
        0,
        D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
        WIC_LOADER_DEFAULT,
        nullptr, m_srv.ReleaseAndGetAddressOf()));
}

Texture2D::~Texture2D()
{
    COM_RELEASE(m_texture);
    COM_RELEASE(m_srv);
}

bool Texture2D::IsPathEmpty(const wchar_t* path) noexcept
{
    return ((path != NULL) && (path[0] == '\0'));
}

void Texture2D::SetInPS(UINT slot) const noexcept
{
    if (!IsValid())
    {
        LOG("--> Texture2D not valid, Failed to set in pixel shader slot " << slot);
        return;
    }

    D3D_CONTEXT->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
}
