#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/UploadScheduler.hpp"

namespace Wl
{

    MaterialRegistry::MaterialRegistry(const SharedPtr<RHIDevice>& device, uint32_t binding)
        : m_device(device)
        , m_shaderBinding(binding)
    {
        RHIShaderResourceBinding materialBinding(m_shaderBinding,
                                                 RHIShaderResourceType::StorageBuffer,
                                                 RHIShaderStage::AllGraphics | RHIShaderStage::Compute,
                                                 MaxResources);

        m_srgPool = m_device->CreateSRGPool(1, {materialBinding});
        m_srgLayout = m_device->CreateSRGLayout({materialBinding});
        m_group = m_srgPool->AllocateSRG(m_srgLayout);

        size_t minAlignment = m_device->GetDeviceProperties().MinStorageBufferOffsetAlignment;
        m_materialStride = Memory::AlignUp(sizeof(MaterialData), minAlignment);

        RHIBufferDescription bufferStorageDescription = {};
        bufferStorageDescription.MemoryUsage = RHIMemoryUsage::Device;
        bufferStorageDescription.SharingMode = RHISharingMode::Private;
        bufferStorageDescription.Usage = RHIBufferUsageFlags::Storage | RHIBufferUsageFlags::TransferDst;
        bufferStorageDescription.Size = m_materialStride * MaxResources;
        m_bufferStorage = m_device->CreateBuffer(bufferStorageDescription);
    }

    void MaterialRegistry::Destroy()
    {
        m_device->DestroyBuffer(m_bufferStorage);
        m_device->DestroySRGPool(m_srgPool);
        m_device->DestroySRGLayout(m_srgLayout);
    }

    MaterialHandle MaterialRegistry::ObtainMaterial(const MaterialData& data)
    {
        MaterialDataKey key = Wl::Hasher::hash(data);
        if (m_cache.Contains(key))
        {
            MaterialHandle handle = m_cache[key];
            if (m_registry[handle] == data)
            {
                return handle;
            }
        }

        WL_CHECK_MSG(m_last < MaxResources, "Material registry is full.");
        MaterialHandle handle = m_last++;
        SetMaterial(handle, data);
        return handle;
    }

    void MaterialRegistry::RemoveMaterial(MaterialHandle handle)
    {
        MaterialDataKey key = m_reverseCache[handle];
        m_reverseCache.Remove(handle);
        m_cache.Remove(key);
        m_registry.Remove(handle);
    }

    void MaterialRegistry::SetMaterial(MaterialHandle handle, const MaterialData& data)
    {
        MaterialDataKey oldKey = m_reverseCache[handle];
        MaterialDataKey key = Wl::Hasher::hash(data);

        if (key != oldKey)
        {
            m_cache.Remove(oldKey);
        }

        m_reverseCache[handle] = key;
        m_cache[key] = handle;
        m_registry[handle] = data;
    }

    const MaterialData& MaterialRegistry::GetMaterial(MaterialHandle handle)
    {
        return m_registry[handle];
    }

    void MaterialRegistry::Upload(SharedPtr<RHICommandQueue> queue)
    {
        size_t minAlignment = m_device->GetDeviceProperties().MinStorageBufferOffsetAlignment;

        UploadScheduler uploader;
        uploader.Init({
                .Device = m_device,
                .StagingSize = m_materialStride * m_last,
                .MinAlignment = minAlignment,
        });

        for (auto [handle, material]: m_registry)
        {
            uploader.Upload(&material, m_materialStride, m_bufferStorage, m_materialStride * handle);
        }

        m_device->ImediateSubmit([&](RHICommandBuffer* commandBuffer)
        {
            double totalUploadedBytes = static_cast<double>(uploader.GetTotalPendingBytes());
            double totalUploadedMegaBytes = totalUploadedBytes / static_cast<double>(WL_KB);
            uploader.Flush(commandBuffer);
            WL_LOG_DEBUG("Renderer", "Flushed global upload scheduler, total uploaded: %.2lfKB", totalUploadedMegaBytes);
        }, queue);

        uploader.Shutdown();
    }

    void MaterialRegistry::CompileShaderResource()
    {
        RHIWriteBufferResource write = {};
        write.Buffer = m_bufferStorage;
        write.Binding = m_shaderBinding;
        write.Range = m_materialStride * m_last;
        m_group->SetBuffer(write);

        m_group->Update();
    }

}// namespace Wl
