#pragma once
#include "WICTextureLoader.h"


class Texture2D
{
public:
	Texture2D(const wchar_t* fileName);
	~Texture2D();

	static bool IsPathEmpty(const wchar_t* path) noexcept;

	const ComPtr<ID3D11ShaderResourceView>& GetSrv() const noexcept { return m_srv; }
	void SetInPS(UINT slot = 0) const noexcept;

	// returns
	bool IsValid() const { return m_srv != nullptr; }

private:
	ComPtr<ID3D11ShaderResourceView> m_srv;
};