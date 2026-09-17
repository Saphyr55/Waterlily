#include "Waterlily/Renderer/Passes/ShadowMapPass.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{
    
    FrameGraphPass& ShadowMapPassCreate(PassContext& passContext,
                                        FramePacket& packet,
                                        GraphicsPipelineState& pipelineState,
                                        ShadowMapPassParameters& paramaters)
    {
        FrameGraphPass& shadowMapPass = passContext.FrameGraph->AddPass(ShadowMapPassName);
        FrameGraphPassDelegate& shadowMapPassDelegate = shadowMapPass.EmplaceDefault();

        shadowMapPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.SetDepthStencil(paramaters.ShadowMap);
        });

        shadowMapPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
        {
            Frame& frame = context.FrameContext->GetCurrentFrame();

            RHIShaderResourceGroupLayout* globalSRGLayout = pipelineState.SRGLayouts[0];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);

            RHIWriteBufferResource writeRenderView(0,
                                                   paramaters.DirectionalLightViewAllocation->Buffer,
                                                   paramaters.DirectionalLightViewAllocation->Offset,
                                                   paramaters.DirectionalLightViewAllocation->Size);
            RHIWriteBufferResource writeRenderInstanceBuffer(1,
                                                   packet.InstanceAllocation.Buffer,
                                                   packet.InstanceAllocation.Offset,
                                                   packet.InstanceAllocation.Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->SetBuffer(writeRenderInstanceBuffer);
            globalSRG->Update();

            RHIGraphicsPipeline* pipeline = passContext.PipelineManager->GetGraphicsPipeline(ShadowMapPassName);

            RHICommandBuffer* commandBuffer = context.CommandBuffer;
            commandBuffer->BindPipeline(pipeline);

            commandBuffer->SetViewport(pipelineState.Viewport);
            commandBuffer->SetScissor(pipelineState.Scissor);

            commandBuffer->BindSRG(pipeline, {globalSRG}, 0);

            FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(paramaters.Indirect);
            
            // We skip drawing if there are nothing to draw.
            if (packet.DrawCount == 0)
            {
                return;
            }

            commandBuffer->BindVertexBuffers(packet.VertexBuffers);
            commandBuffer->BindIndexBuffer(packet.IndexBuffers);

            RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand;
            drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalBuffer.Handle;
            drawIndexedIndirectCommand.Offset = 0;
            drawIndexedIndirectCommand.DrawCount = packet.DrawCount;
            drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

            commandBuffer->Draw(drawIndexedIndirectCommand);
        });

        return shadowMapPass;
    }

}// namespace Wl
