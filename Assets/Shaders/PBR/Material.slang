import Common.Colors;

public typealias TextureHandle = uint;

public static const TextureHandle kInvalidTextureIndex = 0xffffffff;

public struct Material
{
    public float4 baseColorFactor = Colors.White;
    public float metallicFactor = 1.0f;
    public float roughnessFactor = 1.0f;

    public TextureHandle baseColor = kInvalidTextureIndex;
    public TextureHandle normal = kInvalidTextureIndex;
    public TextureHandle emissive = kInvalidTextureIndex;
    public TextureHandle occlusion = kInvalidTextureIndex;
    public TextureHandle metallicRoughness = kInvalidTextureIndex;
}

public struct MaterialBRDF
{
    public float3 F0;
    public float3 albedo;
    public float roughness;
    public float metallic;
}