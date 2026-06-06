
#ifdef __spirv__
    #define WLSL_BINDING(Binding, Set) [[vk::binding(Binding, Set)]]
    #define WLSL_LOCATION(Location) [[vk::location(Location)]]
#else
    #define WLSL_BINDING(Binding, Set)
    #define WLSL_LOCATION(Location)
#endif

#define WLSL_INVALID_TEXTURE_INDEX 0xffffffff

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
