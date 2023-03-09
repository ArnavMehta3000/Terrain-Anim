#include "Includes.hlsli"

float4 PS(PSInput input) : SV_TARGET
{
    return float4(input.TexCoord, 0, 1);
}