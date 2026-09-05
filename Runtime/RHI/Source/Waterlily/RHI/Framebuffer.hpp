#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/TextureView.hpp"

namespace Wl
{

    struct RHIFramebufferDescription
    {
        RHIRenderPass* RenderPass = nullptr;
        Array<RHITextureView*> Attachments;
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t Layers = 1;

        inline bool operator==(const RHIFramebufferDescription& other) const;

        inline bool operator!=(const RHIFramebufferDescription& other) const;
    };

    class RHIFramebuffer
    {
    public:
        virtual const RHIFramebufferDescription& GetDescription() const = 0;

        virtual RHIFramebufferDescription& GetDescription() = 0;

        virtual ~RHIFramebuffer() = default;
    };

}// namespace Wl

namespace std
{
    using namespace Wl;

    template<>
    struct hash<RHIFramebufferDescription>
    {
        size_t operator()(const RHIFramebufferDescription& description) const noexcept
        {
            size_t h = Hash<RHIRenderPass*>()(description.RenderPass);
            for (RHITextureView* view: description.Attachments)
            {
                h ^= Hash<RHITextureView*>()(view);
            }
            h ^= Hash<uint32_t>()(description.Width);
            h ^= Hash<uint32_t>()(description.Height);
            h ^= Hash<uint32_t>()(description.Layers);
            return h;
        }
    };

}// namespace std

inline bool Wl::RHIFramebufferDescription::operator==(const RHIFramebufferDescription& other) const
{
    size_t selfHash = std::hash<RHIFramebufferDescription>()(*this);
    size_t otherHash = std::hash<RHIFramebufferDescription>()(other);
    return selfHash == otherHash;
}

inline bool Wl::RHIFramebufferDescription::operator!=(const RHIFramebufferDescription& other) const
{
    return !(*this == other);
}
