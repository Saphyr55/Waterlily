#include "Passes/LightingPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{
    
    FrameGraphPass& LightingPassCreate(PassContext& passContext, GraphicsPipelineProperties& pipeline, LightingPassParameters& params)
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
            Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
            Rect2D scissor(0.0f, 0.0f, width, height);

            Vector4f color(0.01f, 0.01f, 0.01f, 1.0f);

            RHIShaderResourceGroupLayout* gBufferTexturesSRGLayout = pipeline.SRGLayouts[4];
            RHIShaderResourceGroup* gBufferTexturesSRG = frame.SRGPool->AllocateSRG(gBufferTexturesSRGLayout);
            
            FrameGraphPhysicalTexture& positionResource = context.FrameGraph->ResolvePhysicalTexture(params.Position);
            FrameGraphPhysicalTexture& normalResource = context.FrameGraph->ResolvePhysicalTexture(params.Normal);
            FrameGraphPhysicalTexture& albedoResource = context.FrameGraph->ResolvePhysicalTexture(params.Albedo);

            RHIWriteTextureResource writePosition(0, positionResource.View);
            RHIWriteTextureResource writeNormal(1, normalResource.View);
            RHIWriteTextureResource writeAlbedo(2, albedoResource.View);

            gBufferTexturesSRG->SetTexture(writePosition);
            gBufferTexturesSRG->SetTexture(writeNormal);
            gBufferTexturesSRG->SetTexture(writeAlbedo);

            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(LightingPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(viewport);
                commandBuffer->SetScissor(scissor);
            }

            commandBuffer->EndRenderPass();            
        });

        return lightingPass;
    }

}// namespace Wl
