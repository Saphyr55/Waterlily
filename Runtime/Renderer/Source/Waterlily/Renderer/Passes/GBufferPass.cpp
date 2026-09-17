#include "Waterlily/Renderer/Passes/GBufferPass.hpp"

#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

namespace Wl
{

    FrameGraphPass& GBufferPassCreate(PassContext& passContext,
                                      FramePacket& packet,
                                      GraphicsPipelineState& pipelineState,
                                      GBufferPassParameters& parameters)
    {
        FrameGraphPass& gBufferPass = passContext.FrameGraph->AddPass(GBufferPassName);
        FrameGraphPassDelegate& gBufferPassDelegate = gBufferPass.EmplaceDefault();

        gBufferPassDelegate.SetOnSetup([=, &parameters](FrameGraphPassSetupContext& /* context */, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(parameters.Position);
            builder.Write(parameters.Normal);
            builder.Write(parameters.Albedo);
            builder.Write(parameters.MetallicRoughness);
            builder.ReadStorage(parameters.Indirect);
            builder.SetDepthStencil(parameters.DepthStencil);
        });

        gBufferPassDelegate.SetOnExecute([=, &passContext, &packet, &pipelineState, &parameters](FrameGraphPassExecutionContext& context)
        {
            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[0];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);
            {
                RHIWriteBufferResource writeRenderView(0,
                                                       packet.ViewAllocation.Buffer,
                                                       packet.ViewAllocation.Offset,
                                                       packet.ViewAllocation.Size);

                globalSRG->SetBuffer(writeRenderView);
                globalSRG->Update();
            }

            RHIShaderResourceGroupLayout* renderInstanceSRGLayout = pipelineState.SRGLayouts[1];
            RHIShaderResourceGroup* renderInstanceSRG = frame.SRGPool->AllocateSRG(renderInstanceSRGLayout);
            {
                RHIWriteBufferResource writeRenderInstance(0,
                                                           packet.InstanceAllocation.Buffer,
                                                           packet.InstanceAllocation.Offset,
                                                           packet.InstanceAllocation.Size);

                renderInstanceSRG->SetBuffer(writeRenderInstance);
                renderInstanceSRG->Update();
            }

            RHIShaderResourceGroup* texturesSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialsSRG = passContext.MaterialRegistry->GetSRG();

            RHIGraphicsPipeline* pipeline = passContext.PipelineManager->GetGraphicsPipeline(GBufferPassName);
            commandBuffer->BindPipeline(pipeline);

            commandBuffer->SetViewport(pipelineState.Viewport);
            commandBuffer->SetScissor(pipelineState.Scissor);

            commandBuffer->BindSRG(pipeline, {globalSRG}, 0);
            commandBuffer->BindSRG(pipeline, {renderInstanceSRG}, 1);
            commandBuffer->BindSRG(pipeline, {texturesSRG}, 2);
            commandBuffer->BindSRG(pipeline, {materialsSRG}, 3);

            FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(parameters.Indirect);

            // We skip drawing if there are nothing to draw.
            if (packet.DrawCount == 0)
            {
                return;
            }

            commandBuffer->BindVertexBuffers(packet.VertexBuffers);
            commandBuffer->BindIndexBuffer(packet.IndexBuffers);

            RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand = {};
            drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalBuffer.Handle;
            drawIndexedIndirectCommand.Offset = 0;
            drawIndexedIndirectCommand.DrawCount = packet.DrawCount;
            drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

            commandBuffer->Draw(drawIndexedIndirectCommand);
        });

        return gBufferPass;
    }

}// namespace Wl