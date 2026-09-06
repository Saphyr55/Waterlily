import Common.RenderScene;

static const uint kInvalidTextureIndex = 0xffffffff;

struct Material
{
    float4 DiffuseFactor;
    uint DiffuseIndex;
    uint NormalIndex;
}
