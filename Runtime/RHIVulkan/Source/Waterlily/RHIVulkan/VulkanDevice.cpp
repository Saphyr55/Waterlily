#include "Waterlily/RHIVulkan/vulkanDevice.hpp"

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Deleter.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Fence.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHIVulkan/VulkanBufferPool.hpp"
#include "Waterlily/RHIVulkan/VulkanCommandBuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSetLayout.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSetPool.hpp"
#include "Waterlily/RHIVulkan/VulkanFramebuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanGraphicsPipeline.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderPass.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"
#include "Waterlily/RHIVulkan/VulkanSampler.hpp"
#include "Waterlily/RHIVulkan/VulkanSwapchain.hpp"
#include "Waterlily/RHIVulkan/VulkanSync.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"
#include "Waterlily/RHIVulkan/VulkanTexturePool.hpp"
#include "Waterlily/RHIVulkan/VulkanTextureView.hpp"
#include "Waterlily/RHIVulkan/vulkanBindlessShaderResources.hpp"

namespace Wl
{

    static RHIDeviceProperties InitializeDeviceProperties()
    {
        RHIDeviceProperties deviceProperties;
        VulkanContext& context = VulkanContextGet();
        VkPhysicalDevice physicalDevice = context.PhysicalDevice;
        VkPhysicalDeviceProperties physcicalDeviceProperties = {};
        VulkanAPI::vkGetPhysicalDeviceProperties(physicalDevice, &physcicalDeviceProperties);

        VkPhysicalDeviceLimits& limits = physcicalDeviceProperties.limits;
        deviceProperties.MinUniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
        deviceProperties.MinStorageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;
        deviceProperties.MaxUniformBufferRange = limits.maxUniformBufferRange;
        deviceProperties.MaxStorageBufferRange = limits.maxStorageBufferRange;
        deviceProperties.MaxBoundShaderResourceGroup = limits.maxBoundDescriptorSets;
        deviceProperties.MaxComputeWorkGroupInvocations = limits.maxComputeWorkGroupInvocations;
        deviceProperties.NonCoherentAtomSize = limits.nonCoherentAtomSize;

        return deviceProperties;
    }

    void VulkanDevice::Init()
    {
        m_properties = InitializeDeviceProperties();
    }

    void VulkanDevice::Shutdown()
    {
    }

    const RHIDeviceProperties& VulkanDevice::GetDeviceProperties() const
    {
        return m_properties;
    }

    void VulkanDevice::WaitFence(RHIFence* fence)
    {
        if (fence->IsSignaled())
        {
            return;
        }

        VulkanFence* vulkanFence = static_cast<VulkanFence*>(fence);

        VkResult resultWait =
                VulkanAPI::vkWaitForFences(m_context.Device, 1, &vulkanFence->GetHandle(), VK_TRUE, UINT64_MAX);
        switch (resultWait)
        {
            case VK_SUCCESS: {
                vulkanFence->SetSignaled(true);
                return;
            }
            default: {
                WL_VULKAN_CHECK(resultWait);
                break;
            }
        }
    }

    void VulkanDevice::ResetFence(RHIFence* fence)
    {
        if (!fence->IsSignaled())
        {
            return;
        }

        VulkanFence* vulkanFence = static_cast<VulkanFence*>(fence);
        WL_VULKAN_CHECK(VulkanAPI::vkResetFences(m_context.Device, 1, &vulkanFence->GetHandle()));

        vulkanFence->SetSignaled(false);
    }

    RHIShaderResourceGroupLayout* VulkanDevice::CreateSRGLayout(
            const Array<RHIShaderResourceBinding>& bindings)
    {
        VulkanShaderResourceGroupLayout* vulkanSRGLayout = Wl::New(m_allocator, VulkanShaderResourceGroupLayout());
        vulkanSRGLayout->Create(bindings);
        return vulkanSRGLayout;
    }

    void VulkanDevice::DestroySRGLayout(RHIShaderResourceGroupLayout* layout)
    {
        VulkanShaderResourceGroupLayout* vulkanSRGLayout = static_cast<VulkanShaderResourceGroupLayout*>(layout);
        WL_CHECK(vulkanSRGLayout && vulkanSRGLayout->GetHandle());
        vulkanSRGLayout->Destroy();
        Wl::Delete(m_allocator, vulkanSRGLayout);
    }

    RHIShaderResourceGroupPool* VulkanDevice::CreateSRGPool(
            size_t maxGroupsCount,
            const Array<RHIShaderResourceBinding>& totalBindings)
    {
        VulkanShaderResourceGroupPool* vulkanSRGPool = Wl::New(m_allocator, VulkanShaderResourceGroupPool());
        vulkanSRGPool->Create(maxGroupsCount, totalBindings);
        return vulkanSRGPool;
    }

    void VulkanDevice::DestroySRGPool(RHIShaderResourceGroupPool* pool)
    {
        WL_CHECK(pool);

        VulkanShaderResourceGroupPool* vulkanSRGPool = static_cast<VulkanShaderResourceGroupPool*>(pool);
        vulkanSRGPool->Destroy();
        Wl::Delete(m_allocator, vulkanSRGPool);
    }

    void VulkanDevice::WaitIdle()
    {
        WL_VULKAN_CHECK(VulkanAPI::vkDeviceWaitIdle(m_context.Device));
    }

    RHICommandAllocator* VulkanDevice::CreateCommandAllocatorr(const RHICommandAllocatorDescription& description)
    {
        VulkanCommandAllocator* vulkanCommandAllocator = Wl::New(m_allocator, VulkanCommandAllocator(m_context, description));
        vulkanCommandAllocator->Create();
        vulkanCommandAllocator->AllocateCommandBuffers();
        return vulkanCommandAllocator;
    }

    void VulkanDevice::DestroyCommandAllocator(RHICommandAllocator* commandAllocator)
    {
        WL_CHECK(commandAllocator);

        VulkanCommandAllocator* vulkanCommandAllocator = static_cast<VulkanCommandAllocator*>(commandAllocator);
        vulkanCommandAllocator->Destroy();

        Wl::Delete(m_allocator, vulkanCommandAllocator);
    }

    SharedPtr<RHIBufferPool> VulkanDevice::CreateBufferPool()
    {
        return SharedPtr<VulkanBufferPool>(Wl::New(m_allocator, VulkanBufferPool()),
                                           CreateDeleter<VulkanBufferPool>(m_allocator));
    }

    SharedPtr<RHITexturePool> VulkanDevice::CreateTexturePool()
    {
        return SharedPtr<VulkanTexturePool>(Wl::New(m_allocator, VulkanTexturePool()),
                                            CreateDeleter<RHITexturePool>(m_allocator));
    }

    RHIBindlessShaderResources* VulkanDevice::CreateBindlessShaderResources(
            uint32_t maxResources,
            const Array<RHIShaderResourceBinding>& bindings)
    {
        VulkanBindlessShaderResources* vulkanBindlessShaderResources = Wl::New(m_allocator, VulkanBindlessShaderResources());
        vulkanBindlessShaderResources->Create(maxResources, bindings);
        return vulkanBindlessShaderResources;
    }

    void VulkanDevice::DestroyBindlessShaderResources(RHIBindlessShaderResources* group)
    {
        WL_CHECK(group);

        VulkanBindlessShaderResources* vulkanBindlessShaderResources = static_cast<VulkanBindlessShaderResources*>(group);
        vulkanBindlessShaderResources->Destroy();

        Wl::Delete(m_allocator, vulkanBindlessShaderResources);
    }

    RHITexture* VulkanDevice::CreateTexturre(const RHITextureDescription& description)
    {
        VulkanTexture* vulkanTexture = Wl::New(m_allocator, VulkanTexture());
        vulkanTexture->Create(description);
        return vulkanTexture;
    }

    void VulkanDevice::DestroyTexture(RHITexture* texture)
    {
        VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(texture);
        vulkanTexture->Destroy();
        Wl::Delete(m_allocator, vulkanTexture);
    }

    RHIBuffer* VulkanDevice::CreateBuffer(const RHIBufferDescription& description)
    {
        VulkanBuffer* vulkanBuffer = Wl::New(m_allocator, VulkanBuffer());
        vulkanBuffer->Create(description);
        return vulkanBuffer;
    }

    void VulkanDevice::DestroyBuffer(RHIBuffer* buffer)
    {
        VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(buffer);
        vulkanBuffer->Destroy();
        Wl::Delete(m_allocator, vulkanBuffer);
    }

    RHITextureView* VulkanDevice::CreateTextureView(const RHITextureViewDescription& description)
    {
        VulkanTextureView* vulkanTextureView = New(m_allocator, VulkanTextureView());
        vulkanTextureView->Create(description);
        return vulkanTextureView;
    }

    void VulkanDevice::DestroyTextureView(RHITextureView* textureView)
    {
        WL_CHECK(textureView);

        VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(textureView);
        VulkanAPI::vkDestroyImageView(m_context.Device, vulkanTextureView->GetHandle(), m_context.Allocator);

        Wl::Delete(m_allocator, vulkanTextureView);
    }

    RHISampler* VulkanDevice::CreateSampler(const RHISamplerDescription& description)
    {
        VulkanSampler* vulkanSampler = Wl::New(m_allocator, VulkanSampler());
        vulkanSampler->Create(description);
        return vulkanSampler;
    }

    void VulkanDevice::DestroySampler(RHISampler* sampler)
    {
        WL_CHECK(sampler);

        VulkanSampler* vulkanSampler = static_cast<VulkanSampler*>(sampler);
        vulkanSampler->Destroy();

        Wl::Delete(m_allocator, vulkanSampler);
    }

    RHIRenderPass* VulkanDevice::CreateRenderPass(const RHIRenderPassDescription& description)
    {
        VulkanRenderPass* vulkanRenderPass = Wl::New(m_allocator, VulkanRenderPass(m_context, description));
        vulkanRenderPass->Create();
        return vulkanRenderPass;
    }

    void VulkanDevice::DestroyRenderPass(RHIRenderPass* renderPass)
    {
        WL_CHECK(renderPass);

        VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);
        vulkanRenderPass->Destroy();

        Wl::Delete(m_allocator, vulkanRenderPass);
    }

    RHIGraphicsPipeline* VulkanDevice::CreateGraphicsPipeline(const RHIGraphicsPipelineDescription& description)
    {
        VulkanGraphicsPipeline* vulkanGraphicsPipeline = Wl::New(m_allocator, VulkanGraphicsPipeline());
        vulkanGraphicsPipeline->Create(description);
        return vulkanGraphicsPipeline;
    }

    void VulkanDevice::DestroyGraphicsPipeline(RHIGraphicsPipeline* pipeline)
    {
        VulkanGraphicsPipeline* vulkanGraphicsPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
        vulkanGraphicsPipeline->Destroy();
        Wl::Delete(m_allocator, vulkanGraphicsPipeline);
    }

    RHISwapchain* VulkanDevice::CreateSwapchain(uint32_t width, uint32_t height, uint32_t image_count)
    {
        VulkanSwapchain* vulkanSwapchain = Wl::New(m_allocator, VulkanSwapchain(m_context, width, height, image_count));
        vulkanSwapchain->Create();
        return vulkanSwapchain;
    }

    void VulkanDevice::RecreateSwapchain(RHISwapchain* swapchain, uint32_t width, uint32_t height)
    {
        VulkanSwapchain* vulkanSwapchain = static_cast<VulkanSwapchain*>(swapchain);
        vulkanSwapchain->SetExtent({width, height});
        vulkanSwapchain->Destroy();
        vulkanSwapchain->Create();
    }

    void VulkanDevice::DestroySwapchain(RHISwapchain* swapchain)
    {
        WL_CHECK(swapchain);

        VulkanSwapchain* vulkanSwapchain = static_cast<VulkanSwapchain*>(swapchain);
        vulkanSwapchain->Destroy();

        Wl::Delete(m_allocator, vulkanSwapchain);
    }

    RHIFramebuffer* VulkanDevice::CreateFramebuffer(const RHIFramebufferDescription& description)
    {
        VulkanFramebuffer* vulkanFramebuffer = Wl::New(m_allocator, VulkanFramebuffer(m_context, description));
        vulkanFramebuffer->Create();
        return vulkanFramebuffer;
    }

    void VulkanDevice::DestroyFramebuffer(RHIFramebuffer* framebuffer)
    {
        WL_CHECK(framebuffer);

        VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
        vulkanFramebuffer->Destroy();
        Wl::Delete(m_allocator, vulkanFramebuffer);
    }

    RHISemaphore* VulkanDevice::CreateSemaphore()
    {
        VulkanSemaphore* vulkanSemaphore = Wl::New(m_allocator, VulkanSemaphore());
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        WL_VULKAN_CHECK(VulkanAPI::vkCreateSemaphore(m_context.Device,
                                                     &createInfo,
                                                     m_context.Allocator,
                                                     &vulkanSemaphore->GetHandle()));

        return vulkanSemaphore;
    }

    void VulkanDevice::DestroySemaphore(RHISemaphore* semaphore)
    {
        WL_CHECK(semaphore);

        VulkanSemaphore* vulkanSemaphore = static_cast<VulkanSemaphore*>(semaphore);
        VulkanAPI::vkDestroySemaphore(m_context.Device, vulkanSemaphore->GetHandle(), m_context.Allocator);

        Wl::Delete(m_allocator, vulkanSemaphore);
    }

    RHIFence* VulkanDevice::CreateFence()
    {
        VkFence fence = VK_NULL_HANDLE;
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = VK_NULL_HANDLE;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        WL_VULKAN_CHECK(VulkanAPI::vkCreateFence(m_context.Device, &fenceCreateInfo, m_context.Allocator, &fence));

        return Wl::New(m_allocator, VulkanFence(fence, true));
    }

    void VulkanDevice::DestroyFence(RHIFence* fence)
    {
        WL_CHECK(fence);

        VulkanFence* vulkanFence = static_cast<VulkanFence*>(fence);
        VulkanAPI::vkDestroyFence(m_context.Device, vulkanFence->GetHandle(), m_context.Allocator);

        Wl::Delete(m_allocator, vulkanFence);
    }

    Array<SharedPtr<RHICommandQueue>> VulkanDevice::GetCommandQueues()
    {
        return m_context.CommandQueues;
    }

    VulkanDevice::VulkanDevice()
        : m_context(VulkanContextGet())
        , m_allocator(DefaultAllocator::GetInstance())
    {
    }

}// namespace Wl