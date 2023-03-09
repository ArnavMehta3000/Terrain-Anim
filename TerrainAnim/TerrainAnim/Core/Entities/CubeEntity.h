#pragma once
#include "Core/Entities/Entity.h"

class CubeEntity : public Entity
{
public:
	CubeEntity(const wchar_t* textureFile = L"");
	~CubeEntity();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

private:
	std::unique_ptr<Shader> m_shader;
	ComPtr<ID3D11Buffer>    m_vertexBuffer;
	ComPtr<ID3D11Buffer>    m_indexBuffer;
	Texture2D               m_texture;
	ComPtr<ID3D11Buffer>    m_tessFactorsHS;
	TessellationFactors     m_tessellationFactors;
};