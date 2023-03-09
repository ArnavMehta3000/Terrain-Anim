#pragma once
#include "Core/Scene.h"

struct GridDesc
{
	GridDesc(UINT w, UINT h)
		:
		Width(w),
		Height(h)
	{}

	UINT Width;
	UINT Height;
};

class GridScene : public Scene
{
public:
	GridScene(UINT width, UINT height, const GridDesc& gridDesc);
	~GridScene() = default;

	virtual void Load() override;
	virtual void Update(float dt, const InputEvent& input) override;
	virtual void Render() override;
	virtual void GUI() override;
	virtual void Unload() override;

	void CreateTerrainVB();
	void CreateTerrainIB();

private:
	std::unique_ptr<Shader> m_shader;
	ComPtr<ID3D11Buffer>    m_wvpBuffer;
	ComPtr<ID3D11Buffer>    m_vertexBuffer;
	ComPtr<ID3D11Buffer>    m_indexBuffer;	
	GridDesc                m_gridDesc;
	UINT                    m_indexCount;
};