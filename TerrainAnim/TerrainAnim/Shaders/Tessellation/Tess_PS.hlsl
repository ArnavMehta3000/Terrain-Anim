#include "../Includes.hlsli"

Texture2D tex          : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PSInput input) : SV_TARGET
{
    return tex.Sample(samLinear, input.TexCoord);
}