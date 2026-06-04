#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/Option.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHIColorAttachmentDescription
    {
        RHIFormat Format;

        RHIAttachmentLoadOp LoadOp = RHIAttachmentLoadOp::Clear;
        RHIAttachmentStoreOp StoreOp = RHIAttachmentStoreOp::Store;

        RHIAttachmentLoadOp StencilLoadOp = RHIAttachmentLoadOp::DontCare;
        RHIAttachmentStoreOp StencilStoreOp = RHIAttachmentStoreOp::DontCare;

        RHITextureLayout InitialLayout = RHITextureLayout::Undefined;
        RHITextureLayout FinalLayout = RHITextureLayout::Present;
    };

    struct RHIDepthAttachmentDescription
    {
        RHIFormat Format;

        RHIAttachmentLoadOp LoadOp = RHIAttachmentLoadOp::Clear;
        RHIAttachmentStoreOp StoreOp = RHIAttachmentStoreOp::DontCare;

        RHIAttachmentLoadOp StencilLoadOp = RHIAttachmentLoadOp::Clear;
        RHIAttachmentStoreOp StencilStoreOp = RHIAttachmentStoreOp::DontCare;
    };

    struct RHIRenderPassDescription
    {
        Array<RHIColorAttachmentDescription> ColorAttachmentDecriptions;
        Option<RHIDepthAttachmentDescription> DepthAttachmentDescription = OptionNone<RHIDepthAttachmentDescription>();
    };

    class RHIRenderPass
    {
    public:
        virtual const RHIRenderPassDescription& GetDescription() const = 0;

        virtual ~RHIRenderPass() = default;
    };

}// namespace Wl