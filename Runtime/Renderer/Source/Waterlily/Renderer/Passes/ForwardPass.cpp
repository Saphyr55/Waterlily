#include "Waterlily/Renderer/Passes/ForwardPass.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{

    FrameGraphPass& ForwardPassCreate(PassContext& passContext,
                                      FramePacket& packet,
                                      GraphicsPipelineState& pipelineState,
                                      ForwardPassParameters& paramaters)
    {
        FrameGraphPass& forwardPass = passContext.FrameGraph->AddPass(LudoForwardPassName);
        FrameGraphPassDelegate& forwardPassDelegate = forwardPass.EmplaceDefault();

        forwardPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(paramaters.Color);
            builder.ReadStorage(paramaters.Indirect);
            builder.SetDepthStencil(paramaters.DepthStencil);
        });

        forwardPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
        {
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[0];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);

            RHIWriteBufferResource writeRenderView(0,
                                                   packet.ViewAllocation.Buffer,
                                                   packet.ViewAllocation.Offset,
                                                   packet.ViewAllocation.Size);

            RHIWriteBufferResource writeLight(1,
                                              paramaters.LightAllocation->Buffer,
                                              paramaters.LightAllocation->Offset,
                                              paramaters.LightAllocation->Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->SetBuffer(writeLight);
            globalSRG->Update();

            RHIShaderResourceGroupLayout* renderInstanceSRGLayout = pipelineState.SRGLayouts[1];
            RHIShaderResourceGroup* renderInstanceSRG = frame.SRGPool->AllocateSRG(renderInstanceSRGLayout);

            RHIWriteBufferResource writeRenderInstance(0,
                                                       packet.InstanceAllocation.Buffer,
                                                       packet.InstanceAllocation.Offset,
                                                       packet.InstanceAllocation.Size);

            renderInstanceSRG->SetBuffer(writeRenderInstance);
            renderInstanceSRG->Update();

            RHIShaderResourceGroup* textureSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialSRG = passContext.MaterialRegistry->GetSRG();

            RHIGraphicsPipeline* pipeline = passContext.PipelineManager->GetGraphicsPipeline(LudoForwardPassName);

            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            commandBuffer->BindPipeline(pipeline);

            commandBuffer->SetViewport(pipelineState.Viewport);
            commandBuffer->SetScissor(pipelineState.Scissor);

            commandBuffer->BindSRG(pipeline, {globalSRG}, 0);
            commandBuffer->BindSRG(pipeline, {renderInstanceSRG}, 1);
            commandBuffer->BindSRG(pipeline, {textureSRG}, 2);
            commandBuffer->BindSRG(pipeline, {materialSRG}, 3);

            commandBuffer->BindVertexBuffers(packet.VertexBuffers);
            commandBuffer->BindIndexBuffer(packet.IndexBuffers);

            FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(paramaters.Indirect);

            RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand;
            drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalBuffer.Handle;
            drawIndexedIndirectCommand.Offset = 0;
            drawIndexedIndirectCommand.DrawCount = packet.DrawCount;
            drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

            commandBuffer->Draw(drawIndexedIndirectCommand);
        });

        return forwardPass;
    }

}// namespace Wl
