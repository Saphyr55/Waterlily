#include "Passes/ForwardPass.hpp"

#include "LudoTypes.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{

    FrameGraphPass& ForwardPassCreate(PassContext& passContext,
                                      GraphicsPipelineState& pipeline,
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

            RHIShaderResourceGroupLayout* globalSRGLayout = pipeline.SRGLayouts[GlobalSRGIndex];
            RHIShaderResourceGroup* globalSRG = frame.SRGPool->AllocateSRG(globalSRGLayout);

            RHIWriteBufferResource writeRenderView(GlobalSRGRenderViewBinding,
                                                   params.RenderViewAllocation->Buffer,
                                                   params.RenderViewAllocation->Offset,
                                                   params.RenderViewAllocation->Size);

            RHIWriteBufferResource writeLight(GlobalSRGLightBinding,
                                              params.LightAllocation->Buffer,
                                              params.LightAllocation->Offset,
                                              params.LightAllocation->Size);

            globalSRG->SetBuffer(writeRenderView);
            globalSRG->SetBuffer(writeLight);
            globalSRG->Update();

            RHIShaderResourceGroupLayout* drawItemSRGLayout = pipeline.SRGLayouts[RenderInstanceSRGIndex];
            RHIShaderResourceGroup* drawItemSRG = frame.SRGPool->AllocateSRG(drawItemSRGLayout);

            RHIWriteBufferResource writeDrawItem(RenderInstanceSRGBinding,
                                                 params.MeshAllocation->Buffer,
                                                 params.MeshAllocation->Offset,
                                                 params.MeshAllocation->Size);

            drawItemSRG->SetBuffer(writeDrawItem);
            drawItemSRG->Update();

            RHIShaderResourceGroup* textureSRG = passContext.TextureRegistry->GetSRG();
            RHIShaderResourceGroup* materialSRG = passContext.MaterialRegistry->GetSRG();

            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIPipeline* pipeline = passContext.PipelineManager->GetPipeline(LudoForwardPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(viewport);
                commandBuffer->SetScissor(scissor);

                commandBuffer->BindSRG(pipeline, {globalSRG}, GlobalSRGIndex);
                commandBuffer->BindSRG(pipeline, {drawItemSRG}, RenderInstanceSRGIndex);
                commandBuffer->BindSRG(pipeline, {textureSRG}, LudoTextureGRGIndex);
                commandBuffer->BindSRG(pipeline, {materialSRG}, LudoMaterialsSRGIndex);

                commandBuffer->BindVertexBuffers(params.Mesh->GetVertexBuffers());
                commandBuffer->BindIndexBuffer(params.Mesh->GetIndexBuffer());

                FrameGraphBufferResource& indirectResource = context.FrameGraph->GetBuffer(params.Indirect);

                RHIDrawIndexedIndirectCommand drawIndexedIndirectCommand;
                drawIndexedIndirectCommand.Buffer = indirectResource.PhysicalTexture.Handle;
                drawIndexedIndirectCommand.Offset = 0;
                drawIndexedIndirectCommand.DrawCount = params.DrawCount;
                drawIndexedIndirectCommand.Stride = sizeof(RHIDrawIndexedCommand);

                commandBuffer->Draw(drawIndexedIndirectCommand);
            }

            commandBuffer->EndRenderPass();
        });

        return forwardPass;
    }

}// namespace Wl
