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
	UINT width  = rc.right - rc.left;
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
			if (displayModeList[i].Width == (UINT)width)
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
		swapChainDesc.BufferCount                            = 1;
		swapChainDesc.BufferDesc.Width                       = width;
		swapChainDesc.BufferDesc.Height                      = height;
		swapChainDesc.BufferDesc.Format                      = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if (isVsync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator   = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;  // Lock swapchain refresh rate to 60hz
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}
		swapChainDesc.BufferUsage                            = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.OutputWindow                           = m_hWnd;
		swapChainDesc.SampleDesc.Count                       = 1;  // Anti aliasing here
		swapChainDesc.SampleDesc.Quality                     = 0;
		swapChainDesc.Windowed                               = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering            = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling                     = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect                             = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags                                  = 0;

		D3D_DRIVER_TYPE driverType     = D3D_DRIVER_TYPE_HARDWARE;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		UINT flags                     = 0;
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

	CreateResources(width, height);

	// Create render target view using back buffer as texture resource
	ComPtr<ID3D11Texture2D> backBufferPtr;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferPtr.ReleaseAndGetAddressOf())));
	HR(m_device->CreateRenderTargetView(backBufferPtr.Get(), nullptr, m_backBufferRTV.ReleaseAndGetAddressOf()));
	LOG("Init back buffer");

	// Set defaults
	m_context->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->PSSetSamplers(0, 1, m_samplerAnisotropicWrap.GetAddressOf());
	LOG("Set default primitive topology: [TRIANGLE_LIST]");
	SetWireframe(false);

	// Create viewport
	CREATE_ZERO(D3D11_VIEWPORT, vp);
	vp.Width    = (FLOAT)width;
	vp.Height   = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_context->RSSetViewports(1, &vp);
	LOG("Init viewport");

	LOG("----- Finished Initializing Direct 3D -----");
	return true;
}

void Direct3D::ClearBackBuffer(const std::array<float, 4>& color)
{
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_context->ClearRenderTargetView(m_backBufferRTV.Get(), color.data());
}

void Direct3D::Present()
{
	m_swapChain->Present(0, 0);  // Present as fast as possible
}

void Direct3D::CreateResources(UINT width, UINT height)
{
	// Create depth stencil state
	CREATE_ZERO(D3D11_DEPTH_STENCIL_DESC, dsDesc);
	dsDesc.DepthEnable    = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc      = D3D11_COMPARISON_LESS;
	HR(m_device->CreateDepthStencilState(&dsDesc, m_depthStencilState.ReleaseAndGetAddressOf()));
	LOG("Created depth stencil state");

	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	ID3D11Texture2D* depthBuffer;
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.Width              = width;
	texDesc.Height             = height;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;
	HR(m_device->CreateTexture2D(&texDesc, NULL, &depthBuffer));
	LOG("Created depth texture");

	CREATE_ZERO(D3D11_DEPTH_STENCIL_VIEW_DESC, viewDesc);
	viewDesc.Format             = texDesc.Format;
	viewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(depthBuffer, &viewDesc, m_depthStencilView.ReleaseAndGetAddressOf());

	// Create rasterizer states
	CREATE_ZERO(D3D11_RASTERIZER_DESC, rasterDesc);
	rasterDesc.MultisampleEnable     = TRUE;
	rasterDesc.AntialiasedLineEnable = TRUE;
	rasterDesc.FillMode              = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode              = D3D11_CULL_NONE;
	HR(m_device->CreateRasterizerState(&rasterDesc, m_rasterizerWireframe.ReleaseAndGetAddressOf()));
	LOG("Created wireframe rasterizer state");

	rasterDesc.FillMode              = D3D11_FILL_SOLID;
	rasterDesc.CullMode              = D3D11_CULL_BACK;
	HR(m_device->CreateRasterizerState(&rasterDesc, m_rasterizerSolid.ReleaseAndGetAddressOf()));
	LOG("Created solid rasterizer state");

	// Create sampler
	CREATE_ZERO(D3D11_SAMPLER_DESC, sampDesc);
	sampDesc.Filter         = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD         = 0;
	sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;
	sampDesc.MaxAnisotropy  = 4;
	HR(m_device->CreateSamplerState(&sampDesc, m_samplerAnisotropicWrap.ReleaseAndGetAddressOf()));
	LOG("Created anisotropic wrap sampler state");
}


#pragma region Shader Creation
void Direct3D::CreateVertexShader(std::shared_ptr<VertexShader>& vs, LPCWSTR srcFile, LPCSTR profile, LPCSTR entryPoint)
{
	assert(vs.get() != nullptr);

	vs->Name = srcFile;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		srcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		shaderFlags,
		0,
		vs->Blob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))  // Failed to compile
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
		HR(hr);  // If error halts here, check console for message
	}
	else  // Compiled warnings
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
	}
	COM_RELEASE(errorBlob);

	// Create vertex shader
	HR(m_device->CreateVertexShader(vs->Blob->GetBufferPointer(), vs->Blob->GetBufferSize(), nullptr, vs->Shader.ReleaseAndGetAddressOf()));


	// Create input layout from vertex shader
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11shader/nn-d3d11shader-id3d11shaderreflection
	{
		ComPtr<ID3D11ShaderReflection> vsReflection = nullptr;
		HR(D3DReflect(vs->Blob->GetBufferPointer(),
			vs->Blob->GetBufferSize(),
			IID_PPV_ARGS(vsReflection.ReleaseAndGetAddressOf()))  // Macro usage from https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-iid_ppv_args
		);

		CREATE_ZERO(D3D11_SHADER_DESC, desc);
		HR(vsReflection->GetDesc(&desc));

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout;
		for (UINT i = 0; i < desc.InputParameters; i++)
		{
			// Get input parameter at index
			CREATE_ZERO(D3D11_SIGNATURE_PARAMETER_DESC, paramDesc);
			vsReflection->GetInputParameterDesc(i, &paramDesc);

			// Create input element descripton
			CREATE_ZERO(D3D11_INPUT_ELEMENT_DESC, elementDesc);
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// Determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayout.push_back(elementDesc);
		}

		HR(m_device->CreateInputLayout(
			&inputLayout[0],
			(UINT)inputLayout.size(),
			vs->Blob->GetBufferPointer(),
			vs->Blob->GetBufferSize(),
			vs->InputLayout.ReleaseAndGetAddressOf()
		));

		COM_RELEASE(vsReflection);
	}

	LOG("Created vertex shader from file");
}

void Direct3D::CreatePixelShader(std::shared_ptr<PixelShader>& ps, LPCWSTR srcFile, LPCSTR profile, LPCSTR entryPoint)
{
	assert(ps.get() != nullptr);

	ps->Name = srcFile;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		srcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		shaderFlags,
		0,
		ps->Blob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))  // Failed to compile
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
		HR(hr);
	}
	else  // Compiled warnings
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
	}
	COM_RELEASE(errorBlob);

	HR(m_device->CreatePixelShader(ps->Blob->GetBufferPointer(), ps->Blob->GetBufferSize(), nullptr, ps->Shader.ReleaseAndGetAddressOf()));

	LOG("Created pixel shader from file");
}

void Direct3D::CreateHullShader(std::shared_ptr<HullShader>& hs, LPCWSTR srcFile, LPCSTR profile, LPCSTR entryPoint)
{
	assert(hs.get() != nullptr);

	hs->Name = srcFile;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		srcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		shaderFlags,
		0,
		hs->Blob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))  // Failed to compile
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
		HR(hr);  // If error halts here, check console for message
	}
	else  // Compiled warnings
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
	}
	COM_RELEASE(errorBlob);

	// Create hull shader
	HR(m_device->CreateHullShader(hs->Blob->GetBufferPointer(), hs->Blob->GetBufferSize(), nullptr, hs->Shader.ReleaseAndGetAddressOf()));
	
	LOG("Created hull shader from file");
}

void Direct3D::CreateDomainShader(std::shared_ptr<DomainShader>& ds, LPCWSTR srcFile, LPCSTR profile, LPCSTR entryPoint)
{
	assert(ds.get() != nullptr);

	ds->Name = srcFile;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		srcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		shaderFlags,
		0,
		ds->Blob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))  // Failed to compile
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
		HR(hr);  // If error halts here, check console for message
	}
	else  // Compiled warnings
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
	}
	COM_RELEASE(errorBlob);

	// Create domain shader
	HR(m_device->CreateDomainShader(ds->Blob->GetBufferPointer(), ds->Blob->GetBufferSize(), nullptr, ds->Shader.ReleaseAndGetAddressOf()));

	LOG("Created domain shader from file");
}
#pragma endregion


void Direct3D::CreateConstantBuffer(ComPtr<ID3D11Buffer>& buf, UINT size, D3D11_USAGE usage, UINT cpuAccess)
{
	CREATE_ZERO(D3D11_BUFFER_DESC, cbd);
	cbd.Usage          = usage;
	cbd.ByteWidth      = size;
	cbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = cpuAccess;
	HR(m_device->CreateBuffer(&cbd, nullptr, buf.ReleaseAndGetAddressOf()));
	LOG("Created constant buffer of size: " << size);
}
