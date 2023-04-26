struct VSInput
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct PSInput
{
    float4 Position  : SV_POSITION;
    float4 PositionW : POSITION;
    float2 TexCoord  : TEXCOORD0;
};

struct HS_IO
{
    float4 Position  : SV_POSITION;
    float4 PositionW : POSITION;
    float2 TexCoord  : TEXCOORD0;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float Edges[3] : SV_TessFactor;
    float Inside   : SV_InsideTessFactor;
};

struct TerrainGradients
{
    float Dirt0Height;
    float Dirt1Height;
    float StoneHeight;
    float GrassHeight;
    float SnowHeight;
    float3 _padding;
};