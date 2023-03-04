#include "pch.h"
#include "Graphics/Shaders.h"
#include "Graphics/Direct3D.h"

void VertexShader::Release()
{
	COM_RELEASE(Shader);
	COM_RELEASE(InputLayout);
	COM_RELEASE(Blob);
}

void PixelShader::Release()
{
	COM_RELEASE(Shader);
	COM_RELEASE(Blob);
}

Shader::Shader(LPCWSTR vsFile, LPCWSTR psFile, LPCSTR vsEntry, LPCSTR psEntry)
	:
	m_VertexShader(std::make_shared<VertexShader>()),
	m_PixelShader(std::make_shared<PixelShader>()),
	m_HullShader(nullptr),
	m_DomainShader(nullptr),
	m_isCompleteShader(false)
{
	D3D->CreateVertexShader(m_VertexShader, vsFile, "vs_5_0", vsEntry);
	D3D->CreatePixelShader(m_PixelShader, psFile, "ps_5_0", psEntry);
}

Shader::Shader(const ShaderCreationDesc& desc)
	:
	m_VertexShader(std::make_shared<VertexShader>()),
	m_PixelShader(std::make_shared<PixelShader>()),
	m_HullShader(std::make_shared<HullShader>()),
	m_DomainShader(std::make_shared<DomainShader>()),
	m_isCompleteShader(true)
{
	D3D->CreateVertexShader(m_VertexShader, desc.VertexShaderFile, "vs_5_0", desc.vsEntry);
	D3D->CreatePixelShader(m_PixelShader, desc.PixelShaderFile, "ps_5_0", desc.psEntry);
	D3D->CreateHullShader(m_HullShader, desc.HullShaderFile, "ps_5_0", desc.hsEntry);
	D3D->CreateDomainShader(m_DomainShader, desc.DomainShaderFile, "ps_5_0", desc.dsEntry);
}

void Shader::BindVS(bool applyInputLayout)
{
	D3D_CONTEXT->VSSetShader(m_VertexShader->Shader.Get(), nullptr, 0);
	if (applyInputLayout)
		D3D_CONTEXT->IASetInputLayout(m_VertexShader->InputLayout.Get());
}

void Shader::BindPS()
{
	D3D_CONTEXT->PSSetShader(m_PixelShader->Shader.Get(), nullptr, 0);
}

void Shader::BindAll(bool applyInputLayout)
{
	BindVS(applyInputLayout);
	BindPS();
}

void HullShader::Release()
{
	COM_RELEASE(Shader);
	COM_RELEASE(Blob);
}

void DomainShader::Release()
{
	COM_RELEASE(Shader);
	COM_RELEASE(Blob);
}
