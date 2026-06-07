
struct RenderInstance
{
    float4x4 Model;
    uint MaterialIndex;
};

struct Material
{
    float4 DiffuseFactor;
    uint DiffuseIndex;
    uint NormalIndex;
};

struct PunctualLight
{
    float3 Position;
    float3 Color;
};

struct ViewInstance
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
    float3 EyePosition;
};
