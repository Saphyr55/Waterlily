#include "Waterlily/Renderer/Passes/ForwardPass.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
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
                                      ForwardPassParameters& params)
    {
        FrameGraphPass& forwardPass = passContext.FrameGraph->AddPass(LudoForwardPassName);
        FrameGraphPassDelegate& forwardPassDelegate = forwardPass.EmplaceDefault();

        forwardPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(params.Color);
            builder.ReadStorage(params.Indirect);
            builder.SetDepthStencil(params.DepthStencil);
        });

        forwardPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
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
                                                   packet.ViewAllocation.Buffer,
                                                   packet.ViewAllocation.Offset,
                                                   packet.ViewAllocation.Size);

            RHIWriteBufferResource writeLight(SRGBindingGlobalPointLights,
                                              params.LightAllocation->Buffer,
                                              params.LightAllocation->Offset,
                                              params.LightAllocation->Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->SetBuffer(writeLight);
            globalSRG->Update();

            RHIShaderResourceGroupLayout* renderInstanceSRGLayout = pipelineState.SRGLayouts[SRGIndexRenderInstance];
            RHIShaderResourceGroup* renderInstanceSRG = frame.SRGPool->AllocateSRG(renderInstanceSRGLayout);

            RHIWriteBufferResource writeRenderInstance(SRGBindingRenderInstance,
                                                       packet.InstanceAllocation.Buffer,
                                                       packet.InstanceAllocation.Offset,
                                                       packet.InstanceAllocation.Size);

            renderInstanceSRG->SetBuffer(writeRenderInstance);
            renderInstanceSRG->Update();

            RHIShaderResourceGroup* textureSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialSRG = passContext.MaterialRegistry->GetSRG();

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(LudoForwardPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(viewport);
                commandBuffer->SetScissor(scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, SRGIndexGlobal);
                commandBuffer->BindSRG(pipeline, {renderInstanceSRG}, SRGIndexRenderInstance);
                commandBuffer->BindSRG(pipeline, {textureSRG}, SRGIndexTextures);
                commandBuffer->BindSRG(pipeline, {materialSRG}, SRGIndexMaterials);

                commandBuffer->BindVertexBuffers(packet.VertexBuffers);
                commandBuffer->BindIndexBuffer(packet.IndexBuffers);

                FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(params.Indirect);

                RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand;
                drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalBuffer.Handle;
                drawIndexedIndirectCommand.Offset = 0;
                drawIndexedIndirectCommand.DrawCount = packet.DrawCount;
                drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

                commandBuffer->Draw(drawIndexedIndirectCommand);
            }

            commandBuffer->EndRenderPass();
        });

        return forwardPass;
    }

}// namespace Wl
