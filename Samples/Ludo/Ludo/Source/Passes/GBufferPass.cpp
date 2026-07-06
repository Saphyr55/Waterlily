#include "Passes/GBufferPass.hpp"

#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{

    FrameGraphPass& GBufferPassCreate(PassContext& passContext, 
                                      GraphicsPipelineState& pipelineState,
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

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[SRGIndexGlobal];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);

            RHIWriteBufferResource writeRenderView(SRGBindingGlobalView,
                                                   data.ViewAllocation->Buffer,
                                                   data.ViewAllocation->Offset,
                                                   data.ViewAllocation->Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->Update();

            RHIShaderResourceGroupLayout* drawItemSRGLayout = pipelineState.SRGLayouts[SRGIndexRenderInstance];
            RHIShaderResourceGroup* drawItemSRG = frame.SRGPool->AllocateSRG(drawItemSRGLayout);

            RHIWriteBufferResource writeDrawItem(SRGBindingRenderInstance,
                                                 data.MeshAllocation->Buffer,
                                                 data.MeshAllocation->Offset,
                                                 data.MeshAllocation->Size);

            drawItemSRG->SetBuffer(writeDrawItem);
            drawItemSRG->Update();

            RHIShaderResourceGroup* texturesSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialsSRG = passContext.MaterialRegistry->GetSRG();

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(GBufferPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(viewport);
                commandBuffer->SetScissor(scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, SRGIndexGlobal);
                commandBuffer->BindSRG(pipeline, {drawItemSRG}, SRGIndexRenderInstance);
                commandBuffer->BindSRG(pipeline, {texturesSRG}, SRGIndexTextures);
                commandBuffer->BindSRG(pipeline, {materialsSRG}, SRGIndexMaterials);

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