#pragma once

#include "Waterlily/Core/Containers/ObjectCache.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"

namespace Wl
{

    struct RHIShaderResourceGroupLayoutDescriptionHash
    {
        size_t operator()(const RHIShaderResourceGroupLayoutDescription& description) const
        {
            size_t hash = 0;
            for (const RHIShaderResourceBinding& binding: description.Bindings)
            {
                hash = HashCombine(hash, binding.Binding);
                hash = HashCombine(hash, binding.Count);
                hash = HashCombine(hash, static_cast<size_t>(binding.Type));
                hash = HashCombine(hash, static_cast<size_t>(binding.Stage));
            }
            return hash;
        }
    };

    class RHIShaderResourceGroupLayoutCache : public ObjectCache<RHIShaderResourceGroupLayoutDescription, RHIShaderResourceGroupLayout*, RHIShaderResourceGroupLayoutDescriptionHash>
    {
    public:
        RHIShaderResourceGroupLayoutCache(const SharedPtr<RHIDevice>& device)
            : m_device(device)
        {
            m_createCallback = [&](const RHIShaderResourceGroupLayoutDescription& description) -> RHIShaderResourceGroupLayout*
            {
                return m_device->CreateSRGLayout(description.Bindings);
            };

            m_destroyCallback = [&](RHIShaderResourceGroupLayout*& layout) -> void
            {
                if (layout)
                {
                    m_device->DestroySRGLayout(layout);
                    layout = nullptr;
                }
            };
        }
        ~RHIShaderResourceGroupLayoutCache() = default;

    private:
        SharedPtr<RHIDevice> m_device;
    };

}// namespace Wl