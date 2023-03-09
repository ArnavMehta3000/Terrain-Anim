#include "../Includes.hlsli"

cbuffer TessellationFactors : register(b0)
{
    float EdgeTessFactor = 8.0;
    float InsideTessFactor = 8.0;
    float2 _pad1;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PassThroughConstantHS")]
[maxtessfactor(64.0f)]

HS_IO HS(InputPatch<HS_IO, 3> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    HS_IO output;
    
    output.Position  = ip[i].Position;
    output.PositionW = ip[i].PositionW;
    output.TexCoord  = ip[i].TexCoord;
    
    return output;
}

HS_CONSTANT_DATA_OUTPUT PassThroughConstantHS(InputPatch<HS_IO, 3> ip, uint PatchID : SV_PrimitiveID)
{
    
    HS_CONSTANT_DATA_OUTPUT output;
    output.Edges[0] = EdgeTessFactor;
    output.Edges[1] = EdgeTessFactor;
    output.Edges[2] = EdgeTessFactor;
    output.Inside   = InsideTessFactor;
    
    return output;
}
