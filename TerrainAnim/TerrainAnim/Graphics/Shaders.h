#pragma once

struct VertexShader
{
	VertexShader() = default;
	~VertexShader() { this->Release(); }
	LPCWSTR Name = L"Vertex Shader";

	ComPtr<ID3D11VertexShader> Shader;
	ComPtr<ID3D11InputLayout> InputLayout;
	ComPtr<ID3DBlob> Blob;

	void Release();

};

struct PixelShader
{
	PixelShader() = default;
	~PixelShader() { this->Release(); }

	LPCWSTR Name = L"PixelShader";
	ComPtr<ID3D11PixelShader> Shader;
	ComPtr<ID3DBlob> Blob;

	void Release();
};

struct HullShader
{
	HullShader() = default;
	~HullShader() { this->Release(); }

	LPCWSTR Name = L"HullShader";
	ComPtr<ID3D11HullShader> Shader;
	ComPtr<ID3DBlob> Blob;

	void Release();
};

struct DomainShader
{
	DomainShader() = default;
	~DomainShader() { this->Release(); }

	LPCWSTR Name = L"DomainShader";
	ComPtr<ID3D11DomainShader> Shader;
	ComPtr<ID3DBlob> Blob;

	void Release();
};

struct Shader
{
	struct InitInfo
	{
		LPCWSTR VertexShaderFile;
		LPCWSTR PixelShaderFile;
		LPCWSTR HullShaderFile;
		LPCWSTR DomainShaderFile;

		LPCSTR vsEntry = "VS";
		LPCSTR psEntry = "PS";
		LPCSTR hsEntry = "HS";
		LPCSTR dsEntry = "DS";
	};

	Shader() = default;
	Shader(LPCWSTR vsFile, LPCWSTR psFile, LPCSTR vsEntry = "VS", LPCSTR psEntry = "PS");
	Shader(const InitInfo& desc);
	~Shader() = default;

	void BindVS(bool applyInputLayout);
	void BindPS();
	void BindHS();
	void BindDS();
	void BindAll(bool applyInputLayout = true);

	inline const InitInfo& GetInitInfo() const noexcept { return m_initInfo; }

	std::shared_ptr<VertexShader> m_VertexShader;
	std::shared_ptr<PixelShader>  m_PixelShader;
	std::shared_ptr<HullShader>   m_HullShader;
	std::shared_ptr<DomainShader> m_DomainShader;

private:
	InitInfo m_initInfo;
	bool m_isCompleteShader = false;  // A complete shader contains all 4 shaders
};