#include "Waterlily/Renderer/Passes/LightingPass.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{

    FrameGraphPass& LightingPassCreate(PassContext& passContext,
                                       FramePacket& packet,
                                       GraphicsPipelineState& pipelineState,
                                       LightingPassParameters& params)
    {
        FrameGraphPass& lightingPass = passContext.FrameGraph->AddPass(LightingPassName);
        FrameGraphPassDelegate& lightingPassDelegate = lightingPass.EmplaceDefault();

        lightingPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(params.Color);
            builder.Read(params.Position);
            builder.Read(params.Normal);
            builder.Read(params.Albedo);
            builder.Read(params.MetallicRoughness);
            builder.ReadStorage(params.Indirect);
            builder.SetDepthStencil(params.DepthStencil);
        });

        lightingPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
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
                FrameGraphPhysicalTexture& positionResource = context.FrameGraph->ResolvePhysicalTexture(params.Position);
                FrameGraphPhysicalTexture& normalResource = context.FrameGraph->ResolvePhysicalTexture(params.Normal);
                FrameGraphPhysicalTexture& albedoResource = context.FrameGraph->ResolvePhysicalTexture(params.Albedo);
                FrameGraphPhysicalTexture& metallicRoughnessResource = context.FrameGraph->ResolvePhysicalTexture(params.MetallicRoughness);

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
            float width = static_cast<float>(context.FrameContext->GetWidth());
            float height = static_cast<float>(context.FrameContext->GetHeight());

            Rect2D area(0.0f, 0.0f, width, height);
            Vector4f color(0.01f, 0.01f, 0.01f, 1.0f);

            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);
            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
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
            }

            commandBuffer->EndRenderPass();
        });

        return lightingPass;
    }

}// namespace Wl
