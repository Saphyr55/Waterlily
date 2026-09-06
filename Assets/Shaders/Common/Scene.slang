
public struct RenderInstance
{
    public float4x4 model;
    public uint materialIndex;
}

public struct View
{
    public float4x4 view;
    public float4x4 proj;
    public float4x4 viewProj;
    public float3 position;
}
