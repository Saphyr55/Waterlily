#include "Waterlily/RHIVulkan/VulkanCommandQueue.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHIVulkan/VulkanCommandBuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanSwapchain.hpp"
#include "Waterlily/RHIVulkan/VulkanSync.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    VulkanCommandQueue::VulkanCommandQueue(VulkanContext& context,
                                           VkQueue queue,
                                           RHIQueueType type,
                                           uint32_t queueFamilyIndex,
                                           bool isPresentModeSupported)
        : m_context(context)
        , m_handle(queue)
        , m_type(type)
        , m_queueFamilyIndex(queueFamilyIndex)
        , m_isPresentModeSupported(isPresentModeSupported)
    {
    }

    bool VulkanCommandQueue::IsPresentMode() const
    {
        return m_isPresentModeSupported;
    }

    void VulkanCommandQueue::WaitIdle()
    {
        WL_VULKAN_CHECK(VulkanAPI::vkQueueWaitIdle(m_handle));
    }

    void VulkanCommandQueue::Submit(const Array<RHICommandBuffer*>& commandBuffers,
                                    const Array<RHISemaphore*>& waitSemaphores,
                                    const Array<RHISemaphore*>& signalSemaphores,
                                    const RHIFence* fence) const
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Command buffers
        Array<VkCommandBuffer> vulkanCommandBuffers(commandBuffers.GetSize());
        for (RHICommandBuffer*& commandBuffer: commandBuffers)
        {
            VulkanCommandBuffer* vulkanCommandBufferPtr = static_cast<VulkanCommandBuffer*>(commandBuffer);
            vulkanCommandBuffers.Append(vulkanCommandBufferPtr->GetHandle());
        }

        submitInfo.commandBufferCount = static_cast<uint32_t>(vulkanCommandBuffers.GetSize());
        submitInfo.pCommandBuffers = vulkanCommandBuffers.GetData();

        // Wait semaphores
        Array<VkSemaphore> waitVulkanSemaphores(waitSemaphores.GetSize());
        for (RHISemaphore* semaphore: waitSemaphores)
        {
            waitVulkanSemaphores.Append(static_cast<VulkanSemaphore*>(semaphore)->GetHandle());
        }

        submitInfo.waitSemaphoreCount = waitVulkanSemaphores.GetSize();
        submitInfo.pWaitSemaphores = waitVulkanSemaphores.GetData();

        // Signal semaphores
        Array<VkSemaphore> signalVulkanSemaphores(signalSemaphores.GetSize());
        for (RHISemaphore* semaphore: signalSemaphores)
        {
            signalVulkanSemaphores.Append(static_cast<VulkanSemaphore*>(semaphore)->GetHandle());
        }

        submitInfo.signalSemaphoreCount = signalVulkanSemaphores.GetSize();
        submitInfo.pSignalSemaphores = signalVulkanSemaphores.GetData();

        // Waiting pipeline stage policy
        Array<VkPipelineStageFlags> waitStages(waitSemaphores.GetSize());
        for (size_t i = 0; i < waitSemaphores.GetSize(); i++)
        {
            waitStages.Append(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        }

        submitInfo.pWaitDstStageMask = waitStages.GetData();
        // Fence
        const VulkanFence* vulkanFence = static_cast<const VulkanFence*>(fence);
        VkFence vulkanFenceHandle = vulkanFence ? vulkanFence->GetHandle() : VK_NULL_HANDLE;

        // Submit
        WL_VULKAN_CHECK(VulkanAPI::vkQueueSubmit(m_handle, 1, &submitInfo, vulkanFenceHandle));
    }

    void VulkanCommandQueue::Present(RHISwapchain* swapchain, RHISemaphore* semaphore) const
    {
        VulkanSwapchain* vulkanSwapchain = static_cast<VulkanSwapchain*>(swapchain);
        VulkanSemaphore* vulkanSemaphore = static_cast<VulkanSemaphore*>(semaphore);

        RHISwapchainAcquireResult& result = swapchain->GetCurrentAcquireResult();

        VkSwapchainKHR swapchains[] = {vulkanSwapchain->GetHandle()};

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &vulkanSemaphore->GetHandle();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &result.ImageIndex;

        VkResult qresult = VulkanAPI::vkQueuePresentKHR(m_handle, &presentInfo);
        switch (qresult)
        {
            case VK_SUCCESS: {
                result.IsSuccess = true;
                break;
            }
            case VK_SUBOPTIMAL_KHR: {
                result.IsSuccess = true;
                result.IsSuboptimal = true;
                break;
            }
            case VK_ERROR_OUT_OF_DATE_KHR: {
                result.IsOutOfDate = true;
                break;
            }
            default: {
                WL_VULKAN_CHECK(qresult);
                break;
            }
        }
    }

}// namespace Wl