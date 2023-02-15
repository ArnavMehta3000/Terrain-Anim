#pragma once
#include "Core/Entities/Entity.h"

class CubeEntity : public Entity
{
public:
	CubeEntity();
	~CubeEntity();

	void Update(float dt, const InputEvent& input) override;
	void Render() override;
	void GUI() override;

private:
	std::unique_ptr<Shader> m_shader;
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
};