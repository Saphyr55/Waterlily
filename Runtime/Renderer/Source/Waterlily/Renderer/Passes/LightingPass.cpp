#include "Waterlily/Renderer/Passes/LightingPass.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{

    FrameGraphPass& LightingPassCreate(PassContext& passContext,
                                       FramePacket& packet,
                                       GraphicsPipelineState& pipelineState,
                                       LightingPassParameters& parameters)
    {
        FrameGraphPass& lightingPass = passContext.FrameGraph->AddPass(LightingPassName);
        FrameGraphPassDelegate& lightingPassDelegate = lightingPass.EmplaceDefault();

        lightingPassDelegate.SetOnSetup([=, &parameters](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(parameters.Color);
            builder.Read(parameters.Position);
            builder.Read(parameters.Normal);
            builder.Read(parameters.Albedo);
            builder.Read(parameters.MetallicRoughness);
            builder.ReadStorage(parameters.Indirect);
            builder.SetDepthStencil(parameters.DepthStencil);
        });

        lightingPassDelegate.SetOnExecute([=, &passContext, &packet, &pipelineState, &parameters](FrameGraphPassExecutionContext& context)
        {
            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[0];
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

            RHIShaderResourceGroupLayout* gBufferTexturesSRGLayout = pipelineState.SRGLayouts[1];
            RHIShaderResourceGroup* gBufferTexturesSRG = frame.SRGPool->AllocateSRG(gBufferTexturesSRGLayout);
            {
                FrameGraphPhysicalTexture& positionResource = context.FrameGraph->ResolvePhysicalTexture(parameters.Position);
                FrameGraphPhysicalTexture& normalResource = context.FrameGraph->ResolvePhysicalTexture(parameters.Normal);
                FrameGraphPhysicalTexture& albedoResource = context.FrameGraph->ResolvePhysicalTexture(parameters.Albedo);
                FrameGraphPhysicalTexture& metallicRoughnessResource = context.FrameGraph->ResolvePhysicalTexture(parameters.MetallicRoughness);

                RHISampler* pointSampler = passContext.TextureRegistry->GetDefaultSampler();

                RHIWriteTextureSamplerResource writePosition(0, positionResource.View, pointSampler);
                RHIWriteTextureSamplerResource writeNormal(1, normalResource.View, pointSampler);
                RHIWriteTextureSamplerResource writeAlbedo(2, albedoResource.View, pointSampler);
                RHIWriteTextureSamplerResource writMetallicRoughness(3, metallicRoughnessResource.View, pointSampler);

                gBufferTexturesSRG->SetTextureSampler(writePosition);
                gBufferTexturesSRG->SetTextureSampler(writeNormal);
                gBufferTexturesSRG->SetTextureSampler(writeAlbedo);
                gBufferTexturesSRG->SetTextureSampler(writMetallicRoughness);

                gBufferTexturesSRG->Update();
            }

            RHIGraphicsPipeline* pipeline = passContext.PipelineManager->GetGraphicsPipeline(LightingPassName);

            commandBuffer->BindPipeline(pipeline);

            commandBuffer->SetViewport(pipelineState.Viewport);
            commandBuffer->SetScissor(pipelineState.Scissor);

            commandBuffer->BindSRG(pipeline, {globalSRG}, 0);
            commandBuffer->BindSRG(pipeline, {gBufferTexturesSRG}, 1);

            RHIDrawCommand drawCommand = {};
            drawCommand.FirstInstance = 0;
            drawCommand.FirstVertex = 0;
            drawCommand.InstanceCount = 1;
            drawCommand.VertexCount = 3;
            commandBuffer->Draw(drawCommand);
        });

        return lightingPass;
    }

}// namespace Wl
