#include "Passes/UIPass.hpp"
#include "LudoTypes.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"

namespace Wl
{

    FrameGraphPass& UIPassCreate(PassContext& passContext,
                                 GraphicsPipelineProperties& pipeline,
                                 UIPassParameters& params)
    {
        FrameGraphPass& uiPass = passContext.FrameGraph->AddPass(LudoUIPassName);
        FrameGraphPassDelegate& uiPassDelegate = uiPass.EmplaceDefault();

        uiPassDelegate.SetOnSetup([&](FrameGraphPassSetupContext& context, FrameGraphPassBuilder& builder)
        {
            builder.SetStage(FrameGraphPassStage::Graphics);
            builder.Write(params.Color);
        });

        uiPassDelegate.SetOnExecute([&](FrameGraphPassExecutionContext& context)
        {
            RHICommandBuffer* commandBuffer = context.CommandBuffer;

            RHISwapchain* swapchain = context.FrameContext->GetSwapchain();

            float width = static_cast<float>(swapchain->GetWidth());
            float height = static_cast<float>(swapchain->GetHeight());

            Rect2D area(0.0f, 0.0f, width, height);
            Vector4f color(0.01f, 0.01f, 0.01f, 1.0f);

            RHIRenderPassBeginInfo renderPassBeginInfo = context.CreateRenderPassBeginInfo(color, area);
            commandBuffer->BeginRenderPass(renderPassBeginInfo);
            {
                RHIGraphicsPipeline* pipeline = passContext.PipelineManager->GetPipeline(LudoUIPassName);
                commandBuffer->BindPipeline(pipeline);

                commandBuffer->SetViewport(Viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f));
                commandBuffer->SetScissor(Rect2D(0.0f, 0.0f, width, height));

                commandBuffer->BindVertexBuffers(params.Mesh->GetVertexBuffers());
                commandBuffer->BindIndexBuffer(params.Mesh->GetIndexBuffer());

                RHIDrawIndexedCommand drawIndexedCommand = {};
                drawIndexedCommand.IndexCount = params.Mesh->GetSubMeshCount();
                drawIndexedCommand.InstanceCount = 1;
                commandBuffer->Draw(drawIndexedCommand);
            }
            commandBuffer->EndRenderPass();
        });

        return uiPass;
    }

}// namespace Wl
