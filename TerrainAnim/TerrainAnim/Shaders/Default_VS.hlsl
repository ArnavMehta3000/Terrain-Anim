#include "Includes.hlsli"

cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

VSOutput VS(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    
    float4 pos = float4(input.Position, 1.0f);
    
    output.Position = mul(pos, World);
    output.PositionW = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.TexCoord = input.TexCoord;
    
    return output;
}