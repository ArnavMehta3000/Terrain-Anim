#pragma once
#include "Graphics/Shaders.h"

class Direct3D
{
public:
	Direct3D();
	~Direct3D();
	static std::shared_ptr<Direct3D> Get();

	bool Init(HWND window);
	void ClearBackBuffer(const std::array<float, 4>& color = { 0.0f, 0.0f, 0.0f, 1.0f });
	void Present();

	inline void SetWireframe(bool isWireframe = true) { m_context->RSSetState(isWireframe ? m_rasterizerWireframe.Get() : m_rasterizerSolid.Get()); }

	inline ID3D11Device* GetDevice() const noexcept { return m_device.Get(); }
	inline ID3D11DeviceContext* GetContext() const noexcept { return m_context.Get(); }

	void CreateVertexShader(std::shared_ptr<VertexShader>& vs, LPCWSTR srcFile, LPCSTR profile = "vs_5_0", LPCSTR entryPoint = "VS");
	void CreatePixelShader(std::shared_ptr<PixelShader>& ps, LPCWSTR srcFile, LPCSTR profile = "ps_5_0", LPCSTR entryPoint = "PS");

private:
	void CreateResources(UINT w, UINT h);

private:
	static std::shared_ptr<Direct3D> s_instance;

	HWND m_hWnd;

	ComPtr<ID3D11Device>            m_device;
	ComPtr<ID3D11DeviceContext>     m_context;
	ComPtr<IDXGISwapChain>          m_swapChain;
	ComPtr<ID3D11RenderTargetView>  m_backBufferRTV;
	ComPtr<ID3D11BlendState>        m_blendState;
	ComPtr<ID3D11Texture2D>         m_depthTexture;
	ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	ComPtr<ID3D11RasterizerState> m_rasterizerSolid;
	ComPtr<ID3D11RasterizerState> m_rasterizerWireframe;

	ComPtr<ID3D11SamplerState> m_samplerAnisotropicWrap;
};

#define D3D Direct3D::Get()
#define D3D_DEVICE Direct3D::Get()->GetDevice()
#define D3D_CONTEXT Direct3D::Get()->GetContext()
