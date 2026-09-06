
typealias TextureHandle = uint;

static const TextureHandle kInvalidTextureIndex = 0xffffffff;

struct Material
{
    float4 baseColorFactor;
    float4 metallicFactor;
    float4 roughnessFactor;

    TextureHandle baseColor = kInvalidTextureIndex;
    TextureHandle normal = kInvalidTextureIndex;
    TextureHandle emissive = kInvalidTextureIndex;
    TextureHandle occlusion = kInvalidTextureIndex;
    TextureHandle metallicRoughness = kInvalidTextureIndex;
}
