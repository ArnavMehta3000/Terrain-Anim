#include "Includes.hlsli"

float4 PS(VSOutput input) : SV_TARGET
{
    return float4(input.TexCoord, 0, 1);
}