#include "../Includes.hlsli"

Texture2D dirt0        : register(t0);
Texture2D dirt1        : register(t1);
Texture2D stone        : register(t2);
Texture2D grass        : register(t3);
Texture2D snow         : register(t4);
SamplerState samLinear : register(s0);


cbuffer TextureMix : register(b0)
{
    TerrainGradients GradientValues;
}

float4 DoLerp(Texture2D a, Texture2D b, float t, float2 uv)
{
    return lerp(a.Sample(samLinear, uv), b.Sample(samLinear, uv), t);
}

float4 PS(PSInput input) : SV_TARGET
{
    float height   = input.PositionW.y;
    float gradient = height / 100.0f;
    
    float4 texColor;
    float t = 0.0f;
    if (height < GradientValues.Dirt0Height)
    {
        texColor = dirt0.Sample(samLinear, input.TexCoord);
    }
    else if (height < GradientValues.Dirt1Height)
    {
        t = (height - GradientValues.Dirt0Height) / (GradientValues.Dirt1Height - GradientValues.Dirt0Height);
        texColor = DoLerp(dirt0, dirt1, t, input.TexCoord);
    }
    else if (height < GradientValues.StoneHeight)
    {
        t = (height - GradientValues.Dirt1Height) / (GradientValues.StoneHeight - GradientValues.Dirt1Height);
        texColor = DoLerp(dirt1, stone, t, input.TexCoord);
    }
    else if (height < GradientValues.GrassHeight)
    {
        t = (height - GradientValues.StoneHeight) / (GradientValues.GrassHeight - GradientValues.StoneHeight);
        texColor = DoLerp(stone, grass, t, input.TexCoord);
    }
    else if (height < GradientValues.SnowHeight)
    {
        t = (height - GradientValues.GrassHeight) / (GradientValues.SnowHeight - GradientValues.GrassHeight);
        texColor = DoLerp(grass, snow, t, input.TexCoord);
    }
    else
    {
        texColor = snow.Sample(samLinear, input.TexCoord);
    }
    
    return texColor * gradient;
}