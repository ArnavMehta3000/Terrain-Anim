#include "../Includes.hlsli"

#define MAX_JOINTS 128

cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

cbuffer AnimData : register(b1)
{
    matrix JointsMatrices[MAX_JOINTS];
}


PSInput VS(VSAnimInput input)
{
    PSInput output = (PSInput)0;

    float4 pos = float4(input.Position, 1.0f);

    matrix jointTransfrom = mul(JointsMatrices[input.JointIndices.x], input.JointWeights.x);
    jointTransfrom += mul(JointsMatrices[input.JointIndices.y], input.JointWeights.y);
    jointTransfrom += mul(JointsMatrices[input.JointIndices.z], input.JointWeights.z);
    jointTransfrom += mul(JointsMatrices[input.JointIndices.w], input.JointWeights.w);

    output.Position = mul(pos, jointTransfrom);
    output.Position = mul(pos, World);

    output.PositionW = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.TexCoord = float2(0, 0);
    output.TexCoord = input.TexCoord;

    return output;
}