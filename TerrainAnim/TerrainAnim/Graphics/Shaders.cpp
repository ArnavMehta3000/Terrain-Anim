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

Shader::Shader()
	:
	m_VertexShader(nullptr),
	m_PixelShader(nullptr)
{}

Shader::Shader(LPCWSTR vsFile, LPCWSTR psFile, LPCSTR vsEntry, LPCSTR psEntry)
	:
	m_VertexShader(std::make_shared<VertexShader>()),
	m_PixelShader(std::make_shared<PixelShader>())
{
	D3D->CreateVertexShader(m_VertexShader, vsFile, "vs_5_0", vsEntry);
	D3D->CreatePixelShader(m_PixelShader, psFile, "ps_5_0", psEntry);
}

Shader::~Shader() { }

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

void Shader::BindShader(bool applyInputLayout)
{
	BindVS(applyInputLayout);
	BindPS();
}
