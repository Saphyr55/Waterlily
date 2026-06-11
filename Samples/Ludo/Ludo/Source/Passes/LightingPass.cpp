#include "Passes/LightingPass.hpp"
#include "LudoTypes.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{

    FrameGraphPass& LightingPassCreate(PassContext& passContext, GraphicsPipelineState& pipelineProps, LightingPassParameters& params)
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
            RHISwapchain* swapchain = context.FrameContext->GetSwapchain();
            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            Frame& frame = context.FrameContext->GetCurrentFrame();

            float width = static_cast<float>(swapchain->GetWidth());
            float height = static_cast<float>(swapchain->GetHeight());

            Rect2D area(0.0f, 0.0f, width, height);
            Vector4f color(0.01f, 0.01f, 0.01f, 1.0f);

            uint32_t gBufferSRGGroup = 4;

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineProps.SRGLayouts[GlobalSRGIndex];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);

            RHIWriteBufferResource writeRenderView(GlobalSRGRenderViewBinding,
                                                   params.RenderViewAllocation->Buffer,
                                                   params.RenderViewAllocation->Offset,
                                                   params.RenderViewAllocation->Size);

            RHIWriteBufferResource writeLights(GlobalSRGLightBinding,
                                               params.LightAllocation->Buffer,
                                               params.LightAllocation->Offset,
                                               params.LightAllocation->Size);

            RHIWriteBufferResource writeCounters(2,
                                                 params.CountersAllocation->Buffer,
                                                 params.CountersAllocation->Offset,
                                                 params.CountersAllocation->Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->SetBuffer(writeLights);
            globalSRG->SetBuffer(writeCounters);
            globalSRG->Update();

            RHIShaderResourceGroupLayout* renderInstanceSRGLayout = pipelineProps.SRGLayouts[RenderInstanceSRGIndex];
            RHIShaderResourceGroup* renderInstanceSRG = frame.SRGPool->AllocateSRG(renderInstanceSRGLayout);

            RHIWriteBufferResource writeRenderInstance(RenderInstanceSRGBinding,
                                                       params.MeshAllocation->Buffer,
                                                       params.MeshAllocation->Offset,
                                                       params.MeshAllocation->Size);

            renderInstanceSRG->SetBuffer(writeRenderInstance);
            renderInstanceSRG->Update();

            RHIShaderResourceGroupLayout* gBufferTexturesSRGLayout = pipelineProps.SRGLayouts[gBufferSRGGroup];
            RHIShaderResourceGroup* gBufferTexturesSRG = frame.SRGPool->AllocateSRG(gBufferTexturesSRGLayout);

            FrameGraphPhysicalTexture& positionResource = context.FrameGraph->ResolvePhysicalTexture(params.Position);
            FrameGraphPhysicalTexture& normalResource = context.FrameGraph->ResolvePhysicalTexture(params.Normal);
            FrameGraphPhysicalTexture& albedoResource = context.FrameGraph->ResolvePhysicalTexture(params.Albedo);

            RHISampler* pointSampler = context.FrameContext->GetDefaultSampler();

            RHIWriteTextureSamplerResource writePosition(0, positionResource.View, pointSampler);
            RHIWriteTextureSamplerResource writeNormal(1, normalResource.View, pointSampler);
            RHIWriteTextureSamplerResource writeAlbedo(2, albedoResource.View, pointSampler);

            gBufferTexturesSRG->SetTextureSampler(writePosition);
            gBufferTexturesSRG->SetTextureSampler(writeNormal);
            gBufferTexturesSRG->SetTextureSampler(writeAlbedo);
            gBufferTexturesSRG->Update();

            RHIShaderResourceGroup* textureSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialSRG = passContext.MaterialRegistry->GetSRG();

            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(LightingPassName);

                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(pipelineProps.Viewport);
                commandBuffer->SetScissor(pipelineProps.Scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, GlobalSRGIndex);
                commandBuffer->BindSRG(pipeline, {renderInstanceSRG}, RenderInstanceSRGIndex);
                commandBuffer->BindSRG(pipeline, {textureSRG}, LudoTextureGRGIndex);
                commandBuffer->BindSRG(pipeline, {materialSRG}, LudoMaterialsSRGIndex);
                commandBuffer->BindSRG(pipeline, {gBufferTexturesSRG}, gBufferSRGGroup);

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
