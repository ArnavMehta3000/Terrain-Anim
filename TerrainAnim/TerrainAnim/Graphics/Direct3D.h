#pragma once


class Direct3D
{
public:
	Direct3D();
	~Direct3D();
	static std::shared_ptr<Direct3D> Get();

	bool Init(HWND window);
	void ClearBackBuffer(const std::array<float, 4>& color = { 0.0f, 0.0f, 0.0f, 1.0f });
	void Present();

	inline ID3D11Device* GetDevice() const noexcept { return m_device.Get(); }
	inline ID3D11DeviceContext* GetContext() const noexcept { return m_context.Get(); }

private:
	static std::shared_ptr<Direct3D> s_instance;

	HWND m_hWnd;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
};

#define D3D Direct3D::Get()
#define DEVICE Direct3D::Get()->GetDevice()
#define CONTEXT Direct3D::Get()->GetContext()
