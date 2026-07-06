#include "Passes/LightingPass.hpp"
#include "PassContext.hpp"
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
            builder.ReadStorage(params.Indirect);
            builder.SetDepthStencil(params.DepthStencil);
        });

        lightingPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
        {
            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[SRGIndexGlobal];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);
            {
                RHIWriteBufferResource writeView(SRGBindingGlobalView,
                                                 packet.ViewAllocation.Buffer,
                                                 packet.ViewAllocation.Offset,
                                                  packet.ViewAllocation.Size);

                RHIWriteBufferResource writePointLights(SRGBindingGlobalPointLights,
                                                        params.PointLightsAllocation->Buffer,
                                                        params.PointLightsAllocation->Offset,
                                                        params.PointLightsAllocation->Size);

                RHIWriteBufferResource writeDirectionalLight(SRGBindingGlobalDirectionalLight,
                                                             params.DirectionalLightAllocation->Buffer,
                                                             params.DirectionalLightAllocation->Offset,
                                                             params.DirectionalLightAllocation->Size);

                RHIWriteBufferResource writeCounters(SRGBindingGlobalCounters,
                                                     params.CountersAllocation->Buffer,
                                                     params.CountersAllocation->Offset,
                                                     params.CountersAllocation->Size);

                globalSRG->SetBuffer(writeView);
                globalSRG->SetBuffer(writePointLights);
                globalSRG->SetBuffer(writeDirectionalLight);
                globalSRG->SetBuffer(writeCounters);
                globalSRG->Update();
            }

            RHIShaderResourceGroupLayout* renderInstanceSRGLayout = pipelineState.SRGLayouts[SRGIndexRenderInstance];
            RHIShaderResourceGroup* renderInstanceSRG = frame.SRGPool->AllocateSRG(renderInstanceSRGLayout);
            {
                RHIWriteBufferResource writeRenderInstance(SRGBindingRenderInstance,
                                                           packet.MeshAllocation.Buffer,
                                                           packet.MeshAllocation.Offset,
                                                           packet.MeshAllocation.Size);

                renderInstanceSRG->SetBuffer(writeRenderInstance);
                renderInstanceSRG->Update();
            }

            RHIShaderResourceGroupLayout* gBufferTexturesSRGLayout = pipelineState.SRGLayouts[SRGIndexGBuffer];
            RHIShaderResourceGroup* gBufferTexturesSRG = frame.SRGPool->AllocateSRG(gBufferTexturesSRGLayout);
            {
                FrameGraphPhysicalTexture& positionResource = context.FrameGraph->ResolvePhysicalTexture(params.Position);
                FrameGraphPhysicalTexture& normalResource = context.FrameGraph->ResolvePhysicalTexture(params.Normal);
                FrameGraphPhysicalTexture& albedoResource = context.FrameGraph->ResolvePhysicalTexture(params.Albedo);

                RHISampler* pointSampler = context.FrameContext->GetDefaultSampler();

                RHIWriteTextureSamplerResource writePosition(SRGBindingGBufferPosition, positionResource.View, pointSampler);
                RHIWriteTextureSamplerResource writeNormal(SRGBindingGBufferNormal, normalResource.View, pointSampler);
                RHIWriteTextureSamplerResource writeAlbedo(SRGBindingGBufferAlbedo, albedoResource.View, pointSampler);

                gBufferTexturesSRG->SetTextureSampler(writePosition);
                gBufferTexturesSRG->SetTextureSampler(writeNormal);
                gBufferTexturesSRG->SetTextureSampler(writeAlbedo);
                gBufferTexturesSRG->Update();
            }

            RHIShaderResourceGroup* textureSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialSRG = passContext.MaterialRegistry->GetSRG();

            float width = static_cast<float>(context.FrameContext->GetWidth());
            float height = static_cast<float>(context.FrameContext->GetHeight());

            Rect2D area(0.0f, 0.0f, width, height);
            Vector4f color(0.01f, 0.01f, 0.01f, 1.0f);

            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);
            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(LightingPassName);

                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(pipelineState.Viewport);
                commandBuffer->SetScissor(pipelineState.Scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, SRGIndexGlobal);
                commandBuffer->BindSRG(pipeline, {renderInstanceSRG}, SRGIndexRenderInstance);
                commandBuffer->BindSRG(pipeline, {textureSRG}, SRGIndexTextures);
                commandBuffer->BindSRG(pipeline, {materialSRG}, SRGIndexMaterials);
                commandBuffer->BindSRG(pipeline, {gBufferTexturesSRG}, SRGIndexGBuffer);

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
