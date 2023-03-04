#include "Includes.hlsli"

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PassThroughConstantHS")]
[maxtessfactor(7.0)]

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
    float tessellationFactor = 0.1f;
    
    HS_CONSTANT_DATA_OUTPUT output;
    output.Edges[0] = tessellationFactor;
    output.Edges[1] = tessellationFactor;
    output.Edges[2] = tessellationFactor;
    output.Inside   = tessellationFactor;
    
    return output;
}
