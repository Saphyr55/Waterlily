#pragma once

#include "Waterlily/RHI/TexturePool.hpp"

namespace Wl
{

    class VulkanTexturePool : public RHITexturePool
    {
    public:
        void Init();

        virtual RHITexture* Allocate(const RHITextureDescription& description) override;

        virtual void Deallocate(RHITexture* texture) override;

        virtual void Reset() override;

    public:
        VulkanTexturePool() = default;
        virtual ~VulkanTexturePool() = default;

    private:
    };

}// namespace Wl