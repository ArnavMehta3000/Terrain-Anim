#include "pch.h"
#include "Direct3D.h"

std::shared_ptr<Direct3D> Direct3D::s_instance;

Direct3D::Direct3D()
	:
	m_hWnd(NULL),
	m_device(nullptr),
	m_context(nullptr),
	m_swapChain(nullptr),
	m_backBufferRTV(nullptr)
{
	LOG("Created D3D") 
}

Direct3D::~Direct3D() { LOG("Destructed D3D") }

std::shared_ptr<Direct3D> Direct3D::Get()
{
	if (s_instance == nullptr)
		s_instance = std::make_shared<Direct3D>();
	return std::shared_ptr<Direct3D>(s_instance);
}

bool Direct3D::Init(HWND window)
{
	LOG("----- Initializing Direct 3D -----")
	m_hWnd = window;
	bool isVsync = true;

	// Get window client rect
	CREATE_ZERO(RECT, rc);
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;


	// Core initialize
	{
		ComPtr<IDXGIFactory> factory;
		ComPtr<IDXGIAdapter> adapter;
		ComPtr<IDXGIOutput> adapterOutput;

		DXGI_MODE_DESC* displayModeList;
		UINT numModes = 0, numerator = 0, denominator = 1;

		// Create factory and get display modes
		HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));
		HR(factory->EnumAdapters(0, &adapter));
		HR(adapter->EnumOutputs(0, &adapterOutput));
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

		// Get display modes
		displayModeList = new DXGI_MODE_DESC[numModes];
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

		// Get right display mode from display mode list
		for (unsigned int i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)width)
			{
				if (displayModeList[i].Height == (unsigned int)height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		// Cleanup 1
		delete[] displayModeList;
		displayModeList = 0;
		COM_RELEASE(adapter);
		COM_RELEASE(adapterOutput);


		// Create swapchain description
		CREATE_ZERO(DXGI_SWAP_CHAIN_DESC, swapChainDesc);
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if (isVsync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;  // Lock swapchain refresh rate to 60hz
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.OutputWindow = m_hWnd;
		swapChainDesc.SampleDesc.Count = 1;  // Anti aliasing here
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		auto driverType = D3D_DRIVER_TYPE_HARDWARE;
		auto featureLevel = D3D_FEATURE_LEVEL_11_1;
		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG


		// Create device / swapchain / immediate context
		HR(D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverType,
			NULL,
			flags,
			&featureLevel, 1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_swapChain.ReleaseAndGetAddressOf(),
			m_device.ReleaseAndGetAddressOf(),
			NULL,
			m_context.ReleaseAndGetAddressOf())
		);

		// Prevent Alt+Enter fullscreen
		factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
		COM_RELEASE(factory);
	}
	LOG("Init DirectX 11 Device");
	LOG("Init DirectX 11 Context");
	LOG("Init DirectX 11 Swap Chain");


	// Create render target view using back buffer as texture resource
	ComPtr<ID3D11Texture2D> backBufferPtr;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferPtr.ReleaseAndGetAddressOf())));
	HR(m_device->CreateRenderTargetView(backBufferPtr.Get(), nullptr, m_backBufferRTV.ReleaseAndGetAddressOf()));
	LOG("Init Back Buffer");


	// Create viewport
	CREATE_ZERO(D3D11_VIEWPORT, vp);
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_context->RSSetViewports(1, &vp);
	LOG("Init Viewport");

	LOG("----- Finished Initializing Direct 3D -----");
	return true;
}

void Direct3D::ClearBackBuffer(const std::array<float, 4>& color)
{
	m_context->ClearRenderTargetView(m_backBufferRTV.Get(), color.data());
}

void Direct3D::Present()
{
	m_swapChain->Present(0, 0);  // Present as fast as possible
}
