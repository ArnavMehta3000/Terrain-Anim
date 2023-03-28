#include "../Includes.hlsli"

struct Material
{
    float4 Diffuse;

};

cbuffer MeshData : register(b0)
{
    Material Mat;
}

float4 PS(PSInput input) : SV_TARGET
{
    return Mat.Diffuse;
}