import Common.Lighting;

struct RenderInstance
{
    float4x4 Model;
    uint MaterialIndex;
}

struct View
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
    float3 Position;
}
