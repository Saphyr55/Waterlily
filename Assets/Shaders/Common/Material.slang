import Common.RenderScene;

static const uint kInvalidTextureIndex = 0xffffffff;

struct Material
{
    float4 DiffuseFactor;
    uint DiffuseIndex;
    uint NormalIndex;
}

typealias MaterialRegistry = StructuredBuffer<Material>;

struct TextureRegistry
{
    // TODO: StructuredBuffer<DescriptorHandle<Sampler2D>> registry2D;
    Sampler2D[] registry2D;
}
