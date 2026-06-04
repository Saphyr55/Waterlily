#include "Waterlily/RHI/DeviceMemoryUploader.hpp"

#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/RHI/BufferPool.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/CommandQueue.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/TexturePool.hpp"

namespace Wl
{

    RHIDeviceMemoryUploader::RHIDeviceMemoryUploader(SharedPtr<RHIDevice> device,
                                                     size_t capacity)
        : m_device(device)
        , m_bufferEntries(capacity)
    {
    }

    RHIBufferDescription RHIDeviceMemoryUploader::CreateStagingBufferDescription(const RHIStagingBufferContext& context)
    {
        RHIBufferDescription description = {};
        description.SharingMode = RHISharingMode::Shared;
        description.Usage = RHIBufferUsageFlags::TransferSrc;
        description.MemoryUsage = RHIMemoryUsage::Host;
        description.Size = context.Size;
        return description;
    }

    RHIBufferDescription RHIDeviceMemoryUploader::CreateBufferDescription(const RHIStagingBufferContext& context)
    {
        RHIBufferDescription description = {};
        description.SharingMode = RHISharingMode::Shared;
        description.Usage = context.Usage | RHIBufferUsageFlags::TransferDst;
        description.MemoryUsage = RHIMemoryUsage::Device;
        description.Size = context.Size;
        return description;
    }

    RHITexture* RHIDeviceMemoryUploader::SendTexture(const RHIStagingBufferContext& context,
                                                     RHITextureDescription& description)
    {
        RHIBufferDescription bufferDesc = CreateStagingBufferDescription(context);
        RHIBuffer* staging = m_device->CreateBuffer(bufferDesc);
        m_stangings.Append(staging);

        staging->Update(context.Data, context.Size);

        description.Usage = description.Usage | RHITextureUsageFlags::TransferDst | RHITextureUsageFlags::TransferSrc;
        description.MemoryUsage = RHIMemoryUsage::Device;
        RHITexture* texture = m_device->CreateTexturre(description);

        m_textureEntries.Append(TextureEntry(staging, texture, context.Size));

        return texture;
    }

    RHIBuffer* RHIDeviceMemoryUploader::SendBuffer(const RHIStagingBufferContext& context)
    {
        RHIBufferDescription stagingDescription = CreateStagingBufferDescription(context);
        RHIBuffer* staging = m_device->CreateBuffer(stagingDescription);
        m_stangings.Append(staging);

        staging->Update(context.Data, context.Size);

        RHIBufferDescription bufferDescription = CreateBufferDescription(context);
        RHIBuffer* buffer = m_device->CreateBuffer(bufferDescription);

        m_bufferEntries.Append(BufferEntry(staging, buffer, context.Size));

        return buffer;
    }

    // Upload Data from Standing Buffers to Device Buffers
    void RHIDeviceMemoryUploader::Upload(const SharedPtr<RHICommandQueue>& queue)
    {
        RHICommandAllocatorDescription commandAllocatorDescription = {};
        commandAllocatorDescription.Count = 1;// One command buffer allocated.
        commandAllocatorDescription.CommandQueue = queue;

        RHICommandAllocator* commandAllocator =
                m_device->CreateCommandAllocatorr(commandAllocatorDescription);

        // Open a transfer command buffer
        RHICommandBuffer* commandBuffer = commandAllocator->OpenCommandBuffer();
        commandAllocator->ResetCommandBuffer(commandBuffer);

        commandBuffer->Begin();
        {
            for (auto& [staging, buffer, size]: m_bufferEntries)
            {
                RHICopyBufferCommand copyBufferCommand = {};
                copyBufferCommand.Source = staging;
                copyBufferCommand.Destination = buffer;
                copyBufferCommand.Size = size;
                commandBuffer->CopyBuffer(copyBufferCommand);
            }

            for (auto& entry: m_textureEntries)
            {
                RHITexture* texture = entry.Texture;

                RHITextureLayoutTransition copyBarrier = {};
                copyBarrier.Texture = texture;
                copyBarrier.OldLayout = RHITextureLayout::Undefined;
                copyBarrier.NewLayout = RHITextureLayout::TransferDst;
                commandBuffer->TransitionTextureLayout(copyBarrier);

                RHICopyBufferToTextureCommand copyToTextureCommand = {};
                copyToTextureCommand.Source = entry.Staging;
                copyToTextureCommand.Destination = texture;
                commandBuffer->CopyBufferToTexture(copyToTextureCommand);

                RHITextureLayoutTransition mipmapBarrier = {};
                mipmapBarrier.Texture = texture;
                mipmapBarrier.OldLayout = RHITextureLayout::TransferDst;
                mipmapBarrier.NewLayout = RHITextureLayout::ShaderReadOnly;

                if (texture->GetDescription().MipLevels > 1)
                {
                    // texture_generate_mipmap does the transition.
                    commandBuffer->TextureGenerateMipmap(mipmapBarrier);
                }
                else
                {
                    commandBuffer->TransitionTextureLayout(mipmapBarrier);
                }
            }
        }
        commandBuffer->End();

        // Submit and wait for transfer to complete
        RHIFence* uploadFence = m_device->CreateFence();
        m_device->ResetFence(uploadFence);
        queue->Submit({commandBuffer}, {}, {}, uploadFence);

        m_device->WaitFence(uploadFence);
        m_device->DestroyFence(uploadFence);

        // Standing buffers no longer needed after data upload
        for (RHIBuffer* stanging: m_stangings)
        {
            m_device->DestroyBuffer(stanging);
        }
        m_stangings.Clear();

        m_device->DestroyCommandAllocator(commandAllocator);
    }

}// namespace Wl