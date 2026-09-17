#include "Waterlily/Renderer/Passes/LightingPass.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{

    FrameGraphPass& LightingPassCreate(PassContext& passContext,
                                       FramePacket& packet,
                                       ComputePipelineState& pipelineState,
                                       LightingPassParameters& parameters)
    {
        FrameGraphPass& lightingPass = passContext.FrameGraph->AddPass(LightingPassName);
        FrameGraphPassDelegate& lightingPassDelegate = lightingPass.EmplaceDefault();

        lightingPassDelegate.SetOnSetup([=, &parameters](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Compute);
            builder.WriteStorage(parameters.Color);
            builder.Read(parameters.Position);
            builder.Read(parameters.Normal);
            builder.Read(parameters.Albedo);
            builder.Read(parameters.MetallicRoughness);
            builder.Read(parameters.ShadowMap);
            builder.ReadStorage(parameters.Indirect);
            builder.SetDepthStencil(parameters.DepthStencil);
        });

        lightingPassDelegate.SetOnExecute([=, &passContext, &packet, &pipelineState, &parameters](FrameGraphPassExecutionContext& context)
        {
            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHISampler* pointSampler = passContext.TextureRegistry->GetDefaultSampler();

            constexpr uint32_t globalSRGIndex = 0;
            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[globalSRGIndex];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);
            {
                RHIWriteBufferResource writeView(0,
                                                 packet.ViewAllocation.Buffer,
                                                 packet.ViewAllocation.Offset,
                                                 packet.ViewAllocation.Size);

                RHIWriteBufferResource writeDirectionalLight(1,
                                                             packet.DirectionalLightAllocation.Buffer,
                                                             packet.DirectionalLightAllocation.Offset,
                                                             packet.DirectionalLightAllocation.Size);

                RHIWriteBufferResource writePointLights(2,
                                                        packet.PointLightsAllocation.Buffer,
                                                        packet.PointLightsAllocation.Offset,
                                                        packet.PointLightsAllocation.Size);

                RHIWriteBufferResource writeCounters(3,
                                                     packet.CountersAllocation.Buffer,
                                                     packet.CountersAllocation.Offset,
                                                     packet.CountersAllocation.Size);

                globalSRG->SetBuffer(writeView);
                globalSRG->SetBuffer(writePointLights);
                globalSRG->SetBuffer(writeDirectionalLight);
                globalSRG->SetBuffer(writeCounters);
                globalSRG->Update();
            }

            constexpr uint32_t gBuffersSRGIndex = 1;
            RHIShaderResourceGroupLayout* gBufferTexturesSRGLayout = pipelineState.SRGLayouts[gBuffersSRGIndex];
            RHIShaderResourceGroup* gBufferTexturesSRG = frame.SRGPool->AllocateSRG(gBufferTexturesSRGLayout);
            {
                FrameGraphPhysicalTexture& positionResource = context.FrameGraph->ResolvePhysicalTexture(parameters.Position);
                FrameGraphPhysicalTexture& normalResource = context.FrameGraph->ResolvePhysicalTexture(parameters.Normal);
                FrameGraphPhysicalTexture& albedoResource = context.FrameGraph->ResolvePhysicalTexture(parameters.Albedo);
                FrameGraphPhysicalTexture& metallicRoughnessResource = context.FrameGraph->ResolvePhysicalTexture(parameters.MetallicRoughness);

                RHIWriteTextureSamplerResource writePosition(0, positionResource.View, pointSampler);
                RHIWriteTextureSamplerResource writeNormal(1, normalResource.View, pointSampler);
                RHIWriteTextureSamplerResource writeAlbedo(2, albedoResource.View, pointSampler);
                RHIWriteTextureSamplerResource writMetallicRoughness(3, metallicRoughnessResource.View, pointSampler);

                gBufferTexturesSRG->SetTextureSampler(writePosition);
                gBufferTexturesSRG->SetTextureSampler(writeNormal);
                gBufferTexturesSRG->SetTextureSampler(writeAlbedo);
                gBufferTexturesSRG->SetTextureSampler(writMetallicRoughness);

                FrameGraphPhysicalTexture& colorTexture = context.FrameGraph->ResolvePhysicalTexture(parameters.Color);
                RHIWriteTextureResource writeColor(4, colorTexture.View);
                gBufferTexturesSRG->SetTexture(writeColor);

                gBufferTexturesSRG->Update();
            }

            constexpr uint32_t shadowMapSRGIndex = 2;
            RHIShaderResourceGroupLayout* shadowMapSRGLayout = pipelineState.SRGLayouts[shadowMapSRGIndex];
            RHIShaderResourceGroup* shadowMapSRG = frame.SRGPool->AllocateSRG(shadowMapSRGLayout);
            {
                FrameGraphPhysicalTexture& shadowMapTexture = context.FrameGraph->ResolvePhysicalTexture(parameters.ShadowMap);
                RHIWriteTextureSamplerResource writeShadowMap(0, shadowMapTexture.View, pointSampler);
                shadowMapSRG->SetTextureSampler(writeShadowMap);

                RHIWriteBufferResource writeLigthSpace = RHIWriteBufferResource(1,
                                                     parameters.DirectionalLightSpaceAlloc->Buffer,
                                                     parameters.DirectionalLightSpaceAlloc->Offset,
                                                     parameters.DirectionalLightSpaceAlloc->Size);

                shadowMapSRG->SetBuffer(writeLigthSpace);

                shadowMapSRG->Update();
            }

            RHIPipeline* pipeline = passContext.PipelineManager->GetComputePipeline(LightingPassName);

            commandBuffer->BindPipeline(pipeline);

            commandBuffer->BindSRG(pipeline, {globalSRG}, globalSRGIndex);
            commandBuffer->BindSRG(pipeline, {gBufferTexturesSRG}, gBuffersSRGIndex);
            commandBuffer->BindSRG(pipeline, {shadowMapSRG}, shadowMapSRGIndex);

            RHIDispatchCommand dispatchCommand = {};
            dispatchCommand.GroupCountX = Math::Ceil(context.FrameContext->GetWidth() / 16.0f);
            dispatchCommand.GroupCountY = Math::Ceil(context.FrameContext->GetHeight() / 16.0f);
            dispatchCommand.GroupCountZ = 1;
            commandBuffer->Dispatch(dispatchCommand);
        });

        return lightingPass;
    }

}// namespace Wl
