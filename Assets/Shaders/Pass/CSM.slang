import Common.Light;
import Common.Scene;
import PBR.Material;
import Common.Colors;


[vk::binding(0, 0)]
ConstantBuffer<View> directionLightView;

[vk::binding(1, 0)]
StructuredBuffer<RenderInstance> renderInstanceBuffer;


[shader("vertex")]
float4 VSMain(float3 inputPosition, uint instanceIndex: SV_StartInstanceLocation)
{
    RenderInstance instance = renderInstanceBuffer[instanceIndex];

    float4x4 mvp = mul(directionLightView.viewProj, instance.model);
    float4 position = mul(mvp, float4(inputPosition, 1.0f));

    return position;
}

[shader("fragment")]
float FSMain(float4 position)
{
    // return position.z;
    return Colors.Red.r;
}
