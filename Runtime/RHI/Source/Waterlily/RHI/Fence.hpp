#pragma once

namespace Wl
{

    class RHIFence
    {
    public:
        virtual bool IsSignaled() const = 0;

        virtual ~RHIFence() = default;
    };

}// namespace Wl
