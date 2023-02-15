#include "Includes.hlsli"

float4 PS(VSOutput input) : SV_TARGET
{
    return input.PositionW;
}