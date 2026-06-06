#include "Shared/Defines.hlsli"

struct VSOutput
{
    float4 Position : SV_Position;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;

    float2 position[3] =
    {
        float2(-1.0, -1.0),
        float2(3.0, -1.0),
        float2(-1.0, 3.0)
    };

    output.Position = float4(position[vertexID], 0.0, 1.0);

    return output;
}