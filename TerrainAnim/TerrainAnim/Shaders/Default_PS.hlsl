#include "Includes.hlsli"

float4 PS(PSInput input) : SV_TARGET
{
    return input.PositionW;
}