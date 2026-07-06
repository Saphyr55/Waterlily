#include "Passes/GBufferPass.hpp"

#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

namespace Wl
{

    FrameGraphPass& GBufferPassCreate(PassContext& passContext,
                                      FramePacket& packet,
                                      GraphicsPipelineState& pipelineState,
                                      GBufferPassParameters& data)
    {
        FrameGraphPass& gBufferPass = passContext.FrameGraph->AddPass(GBufferPassName);
        FrameGraphPassDelegate& gBufferPassDelegate = gBufferPass.EmplaceDefault();

        gBufferPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext& /* context */, FrameGraphPassBuilder& builder)
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
            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[SRGIndexGlobal];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);
            {
                RHIWriteBufferResource writeRenderView(SRGBindingGlobalView,
                                                       packet.ViewAllocation.Buffer,
                                                       packet.ViewAllocation.Offset,
                                                       packet.ViewAllocation.Size);

                globalSRG->SetBuffer(writeRenderView);
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

            RHIShaderResourceGroup* texturesSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialsSRG = passContext.MaterialRegistry->GetSRG();

            float width = static_cast<float>(context.FrameContext->GetWidth());
            float height = static_cast<float>(context.FrameContext->GetHeight());

            Rect2D area(0.0f, 0.0f, width, height);
            Vector4f color(0.01f, 0.01f, 0.01f, 1.0f);
            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(GBufferPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(pipelineState.Viewport);
                commandBuffer->SetScissor(pipelineState.Scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, SRGIndexGlobal);
                commandBuffer->BindSRG(pipeline, {renderInstanceSRG}, SRGIndexRenderInstance);
                commandBuffer->BindSRG(pipeline, {texturesSRG}, SRGIndexTextures);
                commandBuffer->BindSRG(pipeline, {materialsSRG}, SRGIndexMaterials);

                commandBuffer->BindVertexBuffers(packet.Mesh->GetVertexBuffers());
                commandBuffer->BindIndexBuffer(packet.Mesh->GetIndexBuffer());

                FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(data.Indirect);

                RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand = {};
                drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalTexture.Handle;
                drawIndexedIndirectCommand.Offset = 0;
                drawIndexedIndirectCommand.DrawCount = packet.DrawCount;
                drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

                commandBuffer->Draw(drawIndexedIndirectCommand);
            }

            commandBuffer->EndRenderPass();
        });

        return gBufferPass;
    }

}// namespace Wl