import Common.Lighting;

struct RenderInstance
{
    float4x4 model;
    uint materialIndex;
}

struct View
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
    float3 position;
}
