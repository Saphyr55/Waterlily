#include "Waterlily/RHIVulkan/VulkanCommandBuffer.hpp"
#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/LinearAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Pipeline.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanBuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanCommandQueue.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSet.hpp"
#include "Waterlily/RHIVulkan/VulkanFramebuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanGraphicsPipeline.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanPipeline.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderPass.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"

namespace Wl
{

    static void PickPipelineStagesAndAccess(VkQueueFlags queueFlags,
                                            VkImageLayout oldLayout,
                                            VkImageLayout newLayout,
                                            VkPipelineStageFlags& outSrcStage,
                                            VkPipelineStageFlags& outDstStage,
                                            VkAccessFlags& outSrcAccess,
                                            VkAccessFlags& outDstAccess)
    {
        outSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        outDstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        outSrcAccess = 0;
        outDstAccess = 0;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            outSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            outDstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            outSrcAccess = 0;
            outDstAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            return;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            outSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            outDstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            outSrcAccess = 0;
            outDstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            return;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            outSrcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            outSrcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            if (queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                outDstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                outDstAccess = VK_ACCESS_SHADER_READ_BIT;
            }
            else if (queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                outDstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                outDstAccess = VK_ACCESS_SHADER_READ_BIT;
            }
            else
            {
                outDstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                outDstAccess = VK_ACCESS_TRANSFER_READ_BIT;
            }
            return;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
                                                       newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
                                                       newLayout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL))
        {
            outSrcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            outDstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            outSrcAccess = 0;
            outDstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            return;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            outSrcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            outDstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            outSrcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            outDstAccess = VK_ACCESS_SHADER_READ_BIT;
            return;
        }

        if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
             oldLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL ||
             oldLayout == VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL))
        {
            outSrcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            outDstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            outSrcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            outDstAccess = VK_ACCESS_SHADER_READ_BIT;
            return;
        }

        if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            {
                outSrcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                outSrcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                outSrcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                outSrcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            else
            {
                outSrcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                outSrcAccess = VK_ACCESS_MEMORY_WRITE_BIT;
            }

            outDstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            outDstAccess = 0;
            return;
        }

        outSrcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        outDstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        outSrcAccess = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        outDstAccess = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    }

    void VulkanCommandBuffer::Begin(RHICommandBufferUsageFlags usage)
    {
        VkCommandBufferUsageFlags flags = 0;

        if ((usage & RHICommandBufferUsageFlags::OneTimeSubmit) == RHICommandBufferUsageFlags::OneTimeSubmit)
        {
            flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }

        if ((usage & RHICommandBufferUsageFlags::RenderPassContinue) ==
            RHICommandBufferUsageFlags::RenderPassContinue)
        {
            flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }

        if ((usage & RHICommandBufferUsageFlags::SimultaneousUse) == RHICommandBufferUsageFlags::SimultaneousUse)
        {
            flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;
        beginInfo.pInheritanceInfo = nullptr;

        WL_VULKAN_CHECK(VulkanAPI::vkBeginCommandBuffer(m_handle, &beginInfo));
    }

    void VulkanCommandBuffer::End()
    {
        WL_VULKAN_CHECK(VulkanAPI::vkEndCommandBuffer(m_handle));
    }

    void VulkanCommandBuffer::BindPipeline(RHIPipeline* pipeline)
    {
        if (pipeline->GetType() == RHIPipeline::Type::Graphics)
        {
            VulkanGraphicsPipeline* vulkanPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
            VulkanAPI::vkCmdBindPipeline(m_handle,
                                         VK_PIPELINE_BIND_POINT_GRAPHICS,
                                         vulkanPipeline->GetHandle().GetPipeline());
        }
    }

    void VulkanCommandBuffer::SetShaderConstants(RHIPipeline* pipeline, const RHIShaderConstants& constants)
    {
        if (pipeline->GetType() == RHIPipeline::Type::Graphics)
        {
            VulkanGraphicsPipeline* vulkanPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
            VkShaderStageFlags stageFlags = VulkanShaderStageGet(constants.Stage);
            VulkanAPI::vkCmdPushConstants(m_handle,
                                          vulkanPipeline->GetHandle().GetPipelineLayout(),
                                          stageFlags,
                                          constants.Offset,
                                          constants.Size,
                                          constants.Data);
        }
    }

    void VulkanCommandBuffer::BindSRG(RHIPipeline* pipeline,
                                      const Array<RHIShaderResourceGroup*>& groups,
                                      size_t groupIndex)
    {
        VulkanPipeline* vulkanPipeline = nullptr;
        if (pipeline->GetType() == RHIPipeline::Type::Graphics)
        {
            VulkanGraphicsPipeline* vulkanGraphicsPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
            vulkanPipeline = &vulkanGraphicsPipeline->GetHandle();
        }

        static constexpr auto mapGroup = [](RHIShaderResourceGroup*& srg) -> VkDescriptorSet
        {
            VulkanShaderResourceGroup* vulkanDescriptorSet = static_cast<VulkanShaderResourceGroup*>(srg);
            return vulkanDescriptorSet->GetHandle();
        };

        Array<VkDescriptorSet> vulkanDescriptorSetArray;
        vulkanDescriptorSetArray.Resize(groups.GetSize());

        Wl::Transform(groups.begin(), groups.end(), vulkanDescriptorSetArray.begin(), mapGroup);

        VulkanAPI::vkCmdBindDescriptorSets(m_handle,
                                           VK_PIPELINE_BIND_POINT_GRAPHICS,
                                           vulkanPipeline->GetPipelineLayout(),
                                           groupIndex,
                                           vulkanDescriptorSetArray.GetSize(),
                                           vulkanDescriptorSetArray.GetData(),
                                           0,
                                           nullptr);
    }

    void VulkanCommandBuffer::BindVertexBuffers(const Array<RHIBuffer*>& buffers)
    {
        Array<VkDeviceSize> offsets;
        offsets.Resize(buffers.GetSize(), 0);

        auto toVulkanBuffer = [](RHIBuffer* buffer) -> VkBuffer
        {
            return static_cast<VulkanBuffer*>(buffer)->GetHandle();
        };

        Array<VkBuffer> vulkanBuffers;
        vulkanBuffers.Resize(buffers.GetSize());

        Wl::Transform(buffers.begin(), buffers.end(), vulkanBuffers.begin(), toVulkanBuffer);

        VulkanAPI::vkCmdBindVertexBuffers(m_handle, 0, buffers.GetSize(), vulkanBuffers.GetData(), offsets.GetData());
    }

    void VulkanCommandBuffer::BindIndexBuffer(RHIBuffer* buffer)
    {
        VkBuffer vulkanBuffer = static_cast<VulkanBuffer*>(buffer)->GetHandle();
        VulkanAPI::vkCmdBindIndexBuffer(m_handle, vulkanBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandBuffer::SetScissors(const Rect2D* scissors, uint32_t count)
    {
        Array<VkRect2D> vulkanScissors;
        vulkanScissors.Resize(count);

        for (size_t i = 0; i < count; i++)
        {
            vulkanScissors[i].offset.x = static_cast<int32_t>(scissors[i].X);
            vulkanScissors[i].offset.y = static_cast<int32_t>(scissors[i].Y);
            vulkanScissors[i].extent.width = static_cast<uint32_t>(scissors[i].Width);
            vulkanScissors[i].extent.height = static_cast<uint32_t>(scissors[i].Height);
        }

        VulkanAPI::vkCmdSetScissor(m_handle, 0, count, vulkanScissors.GetData());
    }

    void VulkanCommandBuffer::SetViewports(const Viewport* viewports, uint32_t count)
    {
        Array<VkViewport> vulkanViewports;
        vulkanViewports.Resize(count);

        for (uint32_t i = 0; i < count; i++)
        {
            vulkanViewports[i].x = viewports[i].X;
            vulkanViewports[i].y = viewports[i].Y;
            vulkanViewports[i].width = viewports[i].Width;
            vulkanViewports[i].height = viewports[i].Height;
            vulkanViewports[i].minDepth = viewports[i].MinDepth;
            vulkanViewports[i].maxDepth = viewports[i].MaxDepth;
        }

        VulkanAPI::vkCmdSetViewport(m_handle, 0, count, vulkanViewports.GetData());
    }

    void VulkanCommandBuffer::TextureGenerateMipmap(const RHITextureLayoutTransition& transition)
    {
        VulkanTexture* texture = static_cast<VulkanTexture*>(transition.Texture);
        VkFormat format = VulkanFormatGet(texture->GetDescription().Format);

        VulkanContext& context = VulkanContextGet();
        uint32_t mipLevels = texture->GetDescription().MipLevels;

        VkFormatProperties formatProperties;
        VulkanAPI::vkGetPhysicalDeviceFormatProperties(context.PhysicalDevice, format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            WL_LOG_ERROR("[Vulkan]", "Texture format does not support linear blitting.");
            return;
        }

        RHITextureLayoutTransition initialTransition = transition;
        initialTransition.NewLayout = RHITextureLayout::TransferDst;
        TransitionTextureLayout(initialTransition);

        int32_t mipWidth = static_cast<int32_t>(texture->GetDescription().Width);
        int32_t mipHeight = static_cast<int32_t>(texture->GetDescription().Height);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = texture->GetHandle();
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            VulkanAPI::vkCmdPipelineBarrier(m_handle,
                                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                                            0,
                                            0,
                                            nullptr,
                                            0,
                                            nullptr,
                                            1,
                                            &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            VulkanAPI::vkCmdBlitImage(m_handle,
                                      texture->GetHandle(),
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                      texture->GetHandle(),
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      1,
                                      &blit,
                                      VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            VulkanAPI::vkCmdPipelineBarrier(m_handle,
                                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                            0,
                                            0,
                                            nullptr,
                                            0,
                                            nullptr,
                                            1,
                                            &barrier);

            if (mipWidth > 1)
            {
                mipWidth /= 2;
            }
            if (mipHeight > 1)
            {
                mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VulkanAPI::vkCmdPipelineBarrier(m_handle,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                        0,
                                        0,
                                        nullptr,
                                        0,
                                        nullptr,
                                        1,
                                        &barrier);

        if (transition.NewLayout != RHITextureLayout::ShaderReadOnly)
        {
            RHITextureLayoutTransition finalTransition = transition;
            finalTransition.OldLayout = RHITextureLayout::ShaderReadOnly;
            TransitionTextureLayout(finalTransition);
        }
    }

    void VulkanCommandBuffer::TransitionTextureLayout(const RHITextureLayoutTransition& transition)
    {
        VulkanTexture* texture = static_cast<VulkanTexture*>(transition.Texture);
        VkImageAspectFlags aspectMask = VulkanImageAspectGet(texture->GetDescription().Format);

        VkImageMemoryBarrier imageBarrier = {};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.oldLayout = VulkanTextureLayoutGet(transition.OldLayout);
        imageBarrier.newLayout = VulkanTextureLayoutGet(transition.NewLayout);
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image = texture->GetHandle();
        imageBarrier.subresourceRange.aspectMask = aspectMask;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = texture->GetDescription().MipLevels;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage = 0;
        VkPipelineStageFlags destinationStage = 0;
        VkAccessFlags sourceAccess = 0;
        VkAccessFlags destinationAccess = 0;

        PickPipelineStagesAndAccess(VulkanQueueTypeGet(m_queueType),
                                    imageBarrier.oldLayout,
                                    imageBarrier.newLayout,
                                    sourceStage,
                                    destinationStage,
                                    sourceAccess,
                                    destinationAccess);

        imageBarrier.srcAccessMask = sourceAccess;
        imageBarrier.dstAccessMask = destinationAccess;

        VulkanAPI::vkCmdPipelineBarrier(m_handle,
                                        sourceStage,
                                        destinationStage,
                                        0,
                                        0,
                                        nullptr,
                                        0,
                                        nullptr,
                                        1,
                                        &imageBarrier);
    }

    void VulkanCommandBuffer::PipelineBarrier(ArrayView<RHITextureBarrier> barriers)
    {
        if (barriers.IsEmpty())
        {
            return;
        }

        Array<VkImageMemoryBarrier> vulkanBarriers(barriers.GetSize());

        VkPipelineStageFlags sourceStages = 0;
        VkPipelineStageFlags destinationStages = 0;

        for (const RHITextureBarrier& barrier: barriers)
        {
            VkImageMemoryBarrier vulkanBarrier{};
            vulkanBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

            vulkanBarrier.oldLayout = VulkanTextureLayoutGet(barrier.OldLayout);
            vulkanBarrier.newLayout = VulkanTextureLayoutGet(barrier.NewLayout);

            vulkanBarrier.srcAccessMask = VulkanAccessGet(barrier.SourceUsage);
            vulkanBarrier.dstAccessMask = VulkanAccessGet(barrier.DestinationUsage);

            vulkanBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vulkanBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(barrier.Texture);
            VkImageAspectFlags aspect_mask = VulkanImageAspectGet(vulkanTexture->GetDescription().Format);

            vulkanBarrier.image = vulkanTexture->GetHandle();

            vulkanBarrier.subresourceRange.aspectMask = aspect_mask;
            vulkanBarrier.subresourceRange.baseMipLevel = barrier.BaseMip;
            vulkanBarrier.subresourceRange.levelCount = barrier.LevelCount;
            vulkanBarrier.subresourceRange.baseArrayLayer = barrier.BaseLayer;
            vulkanBarrier.subresourceRange.layerCount = barrier.LayerCount;

            vulkanBarriers.Append(vulkanBarrier);

            sourceStages |= VulkanPipelineStageGet(barrier.SourceUsage);
            destinationStages |= VulkanPipelineStageGet(barrier.DestinationUsage);
        }

        VulkanAPI::vkCmdPipelineBarrier(m_handle,
                                        sourceStages,
                                        destinationStages,
                                        0,
                                        0,
                                        nullptr,
                                        0,
                                        nullptr,
                                        (uint32_t)vulkanBarriers.GetSize(),
                                        vulkanBarriers.GetData());
    }

    void VulkanCommandBuffer::PipelineBarrier(ArrayView<RHIBufferMemoryBarrier> barriers)
    {
        if (barriers.IsEmpty())
        {
            return;
        }

        Array<VkBufferMemoryBarrier> vulkanBarriers(barriers.GetSize());

        VkPipelineStageFlags sourceStages = 0;
        VkPipelineStageFlags destinationStages = 0;

        for (const RHIBufferMemoryBarrier& barrier: barriers)
        {
            VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(barrier.Buffer);

            VkBufferMemoryBarrier vulkanBarrier = {};
            vulkanBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

            vulkanBarrier.srcAccessMask = VulkanAccessGet(barrier.SourceUsage);
            vulkanBarrier.dstAccessMask = VulkanAccessGet(barrier.DestinationUsage);

            vulkanBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vulkanBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

            vulkanBarrier.buffer = vulkanBuffer->GetHandle();
            vulkanBarrier.offset = barrier.Offset;
            vulkanBarrier.size = barrier.Size == 0 ? VK_WHOLE_SIZE : barrier.Size;

            vulkanBarriers.Append(vulkanBarrier);

            sourceStages |= VulkanPipelineStageGet(barrier.SourceUsage);
            destinationStages |= VulkanPipelineStageGet(barrier.DestinationUsage);
        }

        VulkanAPI::vkCmdPipelineBarrier(m_handle,
                                        sourceStages,
                                        destinationStages,
                                        0,
                                        0,
                                        nullptr,
                                        static_cast<uint32_t>(vulkanBarriers.GetSize()),
                                        vulkanBarriers.GetData(),
                                        0,
                                        nullptr);
    }

    void VulkanCommandBuffer::CopyBufferToTexture(const RHICopyBufferToTextureCommand& command)
    {
        VulkanBuffer* vulkanSource = static_cast<VulkanBuffer*>(command.Source);
        VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(command.Destination);

        VkImage vulkanDestinationImage = vulkanTexture->GetHandle();

        VkImageAspectFlags aspect = VulkanImageAspectGet(vulkanTexture->GetDescription().Format);

        uint32_t width = vulkanTexture->GetDescription().Width;
        uint32_t height = vulkanTexture->GetDescription().Height;

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        // Image Subresource
        region.imageSubresource.aspectMask = aspect;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        // Image Region
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
                width,
                height,
                1,
        };

        VulkanAPI::vkCmdCopyBufferToImage(m_handle,
                                          vulkanSource->GetHandle(),
                                          vulkanDestinationImage,
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          1,
                                          &region);
    }

    void VulkanCommandBuffer::CopyBuffer(const RHICopyBufferCommand& command)
    {
        VulkanBuffer* vulkanSource = static_cast<VulkanBuffer*>(command.Source);
        VulkanBuffer* vulkanDestination = static_cast<VulkanBuffer*>(command.Destination);

        VkBufferCopy buffer_copy = {};
        buffer_copy.srcOffset = command.SourceOffset;
        buffer_copy.dstOffset = command.DestinationOffset;
        buffer_copy.size = command.Size;

        VulkanAPI::vkCmdCopyBuffer(m_handle, vulkanSource->GetHandle(), vulkanDestination->GetHandle(), 1, &buffer_copy);
    }

    void VulkanCommandBuffer::BeginRenderPass(const RHIRenderPassBeginInfo& begin_info)
    {
        VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(begin_info.RenderPass);
        VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(begin_info.Framebuffer);

        const RHIRenderPassDescription& renderPassDescription = vulkanRenderPass->GetDescription();
        uint32_t totalAttachmentCount = renderPassDescription.ColorAttachmentDecriptions.GetSize();

        if (totalAttachmentCount == 0)
        {
            VkRenderPassBeginInfo beginInfo = {};
            VulkanAPI::vkCmdBeginRenderPass(m_handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
            return;
        }

        VkClearColorValue defaultClearColor = {
                begin_info.Color.x,
                begin_info.Color.y,
                begin_info.Color.z,
                begin_info.Color.w,
        };

        VkClearDepthStencilValue depthStencilValue = {};
        depthStencilValue.depth = begin_info.Depth;
        depthStencilValue.stencil = begin_info.Stencil;

        bool hasDepthAttachment = renderPassDescription.DepthAttachmentDescription.HasValue();
        size_t depthOneOrZero = hasDepthAttachment ? 1 : 0;
        uint32_t colorAttachmentCount = totalAttachmentCount;

        Array<VkClearValue> clearValues(totalAttachmentCount + depthOneOrZero);

        for (uint32_t i = 0; i < colorAttachmentCount; i++)
        {
            VkClearValue clearColorValue = {};
            clearColorValue.color = defaultClearColor;
            clearValues.Append(clearColorValue);
        }

        if (hasDepthAttachment)
        {
            VkClearValue clearDepthValue = {};
            clearDepthValue.depthStencil = depthStencilValue;
            clearValues.Append(clearDepthValue);
        }

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = vulkanRenderPass->GetHandle();
        beginInfo.framebuffer = vulkanFramebuffer->GetHandle();
        beginInfo.renderArea.offset = {static_cast<int32_t>(begin_info.Area.X),
                                       static_cast<int32_t>(begin_info.Area.Y)};
        beginInfo.renderArea.extent = {static_cast<uint32_t>(begin_info.Area.Width),
                                       static_cast<uint32_t>(begin_info.Area.Height)};

        beginInfo.clearValueCount = clearValues.GetSize();
        beginInfo.pClearValues = clearValues.GetData();

        VulkanAPI::vkCmdBeginRenderPass(m_handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::EndRenderPass()
    {
        VulkanAPI::vkCmdEndRenderPass(m_handle);
    }

    void VulkanCommandBuffer::Draw(const RHIDrawCommand& command)
    {
        VulkanAPI::vkCmdDraw(m_handle,
                             command.VertexCount,
                             command.InstanceCount,
                             command.FirstVertex,
                             command.FirstInstance);
    }

    void VulkanCommandBuffer::Draw(const RHIDrawIndexedCommand& command)
    {
        VulkanAPI::vkCmdDrawIndexed(m_handle,
                                    command.IndexCount,
                                    command.InstanceCount,
                                    command.FirstIndex,
                                    command.VertexOffset,
                                    command.FirstInstance);
    }

    void VulkanCommandBuffer::Draw(const RHIDrawIndexedIndirectCommand& command)
    {
        VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(command.Buffer);
        VulkanAPI::vkCmdDrawIndexedIndirect(m_handle,
                                            vulkanBuffer->GetHandle(),
                                            command.Offset,
                                            command.DrawCount,
                                            command.Stride);
    }

    RHICommandBuffer* VulkanCommandAllocator::OpenCommandBuffer(uint32_t index)
    {
        return m_upperCommandBuffers[index];
    }

    void VulkanCommandAllocator::ResetCommandBuffer(RHICommandBuffer* commandBuffer)
    {
        VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
        VulkanAPI::vkResetCommandBuffer(vulkanCommandBuffer->GetHandle(), 0);
    }

    VkCommandPool& VulkanCommandAllocator::GetHandle()
    {
        return m_handle;
    }

    const Array<VkCommandBuffer>& VulkanCommandAllocator::GetCommandBuffers() const
    {
        return m_commandBuffers;
    }

    void VulkanCommandAllocator::Create()
    {
        VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if ((m_queueType & RHIQueueType::Transfer) == RHIQueueType::Transfer)
        {
            flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        }

        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = flags;
        createInfo.queueFamilyIndex = m_queueFamilyIndex;

        WL_VULKAN_CHECK(VulkanAPI::vkCreateCommandPool(m_context.Device,
                                                       &createInfo,
                                                       m_context.Allocator,
                                                       &m_handle));
    }

    void VulkanCommandAllocator::AllocateCommandBuffers()
    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_handle;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = m_count;

        WL_VULKAN_CHECK(
                VulkanAPI::vkAllocateCommandBuffers(m_context.Device, &allocateInfo, m_commandBuffers.GetData()));

        for (size_t i = 0; i < m_count; i++)
        {
            m_upperCommandBuffers[i] =
                    Wl::New(m_upperCommandBufferAllocator, VulkanCommandBuffer(m_commandBuffers[i], m_queueType));
        }
    }

    void VulkanCommandAllocator::Destroy()
    {
        m_upperCommandBufferAllocator.Destroy();
        VulkanAPI::vkDestroyCommandPool(m_context.Device, m_handle, m_context.Allocator);
    }

    VulkanCommandAllocator::VulkanCommandAllocator(VulkanContext& context,
                                                   const RHICommandAllocatorDescription& description)
        : m_context(context)
        , m_count(description.Count)
        , m_queueType(description.CommandQueue->GetQueueType())
        , m_queueFamilyIndex(1)
        , m_handle(VK_NULL_HANDLE)
        , m_commandBuffers()
    {
        m_commandBuffers.Resize(m_count);
        m_upperCommandBuffers.Resize(m_count);
        m_upperCommandBufferAllocator.Init(m_count * sizeof(VulkanCommandBuffer));
        m_queueFamilyIndex = Wl::StaticPtrCast<VulkanCommandQueue>(description.CommandQueue)->GetQueueFamilyIndex();
    }

}// namespace Wl