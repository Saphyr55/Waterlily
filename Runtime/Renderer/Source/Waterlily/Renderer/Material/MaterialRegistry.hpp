#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/CommandQueue.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/ShaderResourcePool.hpp"
#include "Waterlily/Renderer/Material/Material.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    using MaterialHandle = uint16_t;

    class WL_RENDERER_API MaterialRegistry
    {
    public:
        inline static constexpr MaterialHandle InvalidHandle = UINT16_MAX;
        inline static constexpr uint16_t MaxResources = 4096;

        MaterialHandle ObtainMaterial(const MaterialData& data = {});
        void RemoveMaterial(MaterialHandle handle);
        void SetMaterial(MaterialHandle handle, const MaterialData& data);
        const MaterialData& GetMaterial(MaterialHandle handle);

        void Upload(SharedPtr<RHICommandQueue> queue);
        void CompileShaderResource();

        inline uint32_t GetShaderBinding()
        {
            return m_shaderBinding;
        }

        inline RHIShaderResourceGroupLayout* GetSRGLayout()
        {
            return m_srgLayout;
        }

        inline RHIShaderResourceGroup* GetSRG()
        {
            return m_group;
        }

    public:
        MaterialRegistry(const SharedPtr<RHIDevice>& device, uint32_t binding);
        ~MaterialRegistry();

    private:
        void Destroy();

    private:
        SharedPtr<RHIDevice> m_device;
        RHIShaderResourceGroupLayout* m_srgLayout;
        RHIShaderResourceGroup* m_group;
        RHIShaderResourceGroupPool* m_srgPool;
        RHIBuffer* m_bufferStorage;

        using MaterialDataKey = size_t;

        HashMap<MaterialDataKey, MaterialHandle> m_cache;
        HashMap<MaterialHandle, MaterialDataKey> m_reverseCache;
        HashMap<MaterialHandle, MaterialData> m_registry;

        MaterialHandle m_last = 0;
        uint32_t m_shaderBinding;

        size_t m_materialStride = 0;
    };

}// namespace Wl
