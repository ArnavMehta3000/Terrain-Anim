#include "../Includes.hlsli"

cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

cbuffer TerrainDataBuffer : register(b1)
{
    TerrainData TerrainDataValues;
};

Texture2D noiseMap         : register(t0);
SamplerState linearSampler : register(s0);

[domain("tri")]
PSInput DS(HS_CONSTANT_DATA_OUTPUT input, float3 bcCoords : SV_DomainLocation, const OutputPatch<HS_IO, 3> TrianglePatch)
{
    PSInput output = (PSInput)0;
    

    float3 worldPos = bcCoords.x * TrianglePatch[0].Position.xyz +
                      bcCoords.y * TrianglePatch[1].Position.xyz +
                      bcCoords.z * TrianglePatch[2].Position.xyz;
    
    output.TexCoord = bcCoords.x * TrianglePatch[0].TexCoord +
                      bcCoords.y * TrianglePatch[1].TexCoord +
                      bcCoords.z * TrianglePatch[2].TexCoord;
    
    // Remap noise
    if (TerrainDataValues.UseHeightMap)
    {
        float2 texCoord = output.TexCoord.xy / float2(TerrainDataValues.NoiseWidth, TerrainDataValues.NoiseHeight);
        float noiseValue = (noiseMap.SampleLevel(linearSampler, texCoord, 0).r * 2.0f) - 1.0f;
        float noiseY = noiseValue * TerrainDataValues.HeightMultiplier;
        worldPos.y += noiseY;
    }
    
    output.Position  = mul(float4(worldPos, 1.0f), World);
    output.PositionW = output.Position;
    output.Position  = mul(output.Position, View);
    output.Position  = mul(output.Position, Projection);
    
    return output;
}
