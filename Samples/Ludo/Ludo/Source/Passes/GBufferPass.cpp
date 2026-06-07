#include "Passes/GBufferPass.hpp"

#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "LudoTypes.hpp"

namespace Wl
{

    FrameGraphPass& GBufferPassCreate(PassContext& passContext, 
                                      GraphicsPipelineProperties& pipeline,
                                      GBufferPassParameters& data)
    {
        FrameGraphPass& gBufferPass = passContext.FrameGraph->AddPass(GBufferPassName);
        FrameGraphPassDelegate & gBufferPassDelegate = gBufferPass.EmplaceDefault();

        gBufferPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext&, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(data.Position);
            builder.Write(data.Normal);
            builder.Write(data.Albedo);
            builder.ReadStorage(data.Indirect);
            builder.SetDepthStencil(data.DepthStencil);
        });

        gBufferPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
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

            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);

            RHIShaderResourceGroupLayout* globalSRGLayout = pipeline.SRGLayouts[GlobalSRGIndex];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);

            RHIWriteBufferResource writeRenderView(GlobalSRGRenderViewBinding,
                                                   data.RenderViewAllocation->Buffer,
                                                   data.RenderViewAllocation->Offset,
                                                   data.RenderViewAllocation->Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->Update();

            RHIShaderResourceGroupLayout* drawItemSRGLayout = pipeline.SRGLayouts[RenderInstanceSRGIndex];
            RHIShaderResourceGroup* drawItemSRG = frame.SRGPool->AllocateSRG(drawItemSRGLayout);

            RHIWriteBufferResource writeDrawItem(RenderInstanceSRGBinding,
                                                 data.MeshAllocation->Buffer,
                                                 data.MeshAllocation->Offset,
                                                 data.MeshAllocation->Size);

            drawItemSRG->SetBuffer(writeDrawItem);
            drawItemSRG->Update();

            RHIShaderResourceGroup* textureSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialSRG = passContext.MaterialRegistry->GetSRG();

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(GBufferPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(viewport);
                commandBuffer->SetScissor(scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, GlobalSRGIndex);
                commandBuffer->BindSRG(pipeline, {drawItemSRG}, RenderInstanceSRGIndex);
                commandBuffer->BindSRG(pipeline, {textureSRG}, LudoTextureGRGIndex);
                commandBuffer->BindSRG(pipeline, {materialSRG}, LudoMaterialsSRGIndex);

                commandBuffer->BindVertexBuffers(data.Mesh->GetVertexBuffers());
                commandBuffer->BindIndexBuffer(data.Mesh->GetIndexBuffer());

                FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(data.Indirect);

                RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand = {};
                drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalTexture.Handle;
                drawIndexedIndirectCommand.Offset = 0;
                drawIndexedIndirectCommand.DrawCount = data.DrawCount;
                drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

                commandBuffer->Draw(drawIndexedIndirectCommand);
            }

            commandBuffer->EndRenderPass();
        });

        return gBufferPass;
    }

}