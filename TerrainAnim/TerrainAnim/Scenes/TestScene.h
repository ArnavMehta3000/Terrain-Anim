#pragma once
#include "Core/Scene.h"

class TestScene : public Scene
{
public:
	TestScene(UINT width, UINT height);
	~TestScene() {}

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void Unload() override;

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	ComPtr<ID3D11Buffer> m_wvpBuffer;
	std::unique_ptr<Shader> m_shader;
};