#pragma once
#include "WICTextureLoader.h"


class Texture2D
{
public:
	Texture2D(const wchar_t* fileName);
	~Texture2D();

	static bool IsPathEmpty(const wchar_t* path) noexcept;

	const ComPtr<ID3D11ShaderResourceView>& GetSrv()  const noexcept { return m_srv; }
	const ComPtr<ID3D11Texture2D>&          GetTex()  const noexcept { return m_texture; }
	const D3D11_TEXTURE2D_DESC&             GetDesc() const noexcept { return m_desc; }
	
	void SetInPS(UINT slot = 0) const noexcept;


	// returns
	bool IsValid() const { return m_srv != nullptr; }

private:
	D3D11_TEXTURE2D_DESC             m_desc;
	ComPtr<ID3D11Texture2D>          m_texture;
	ComPtr<ID3D11ShaderResourceView> m_srv;
};