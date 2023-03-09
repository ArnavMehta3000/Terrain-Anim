#include "../Includes.hlsli"
PSInput VS(VSInput input)
{
    PSInput output = (PSInput) 0;
    
    float4 pos      = float4(input.Position, 1.0f);
    output.Position = pos;
    output.TexCoord = input.TexCoord;
    
    return output;
}