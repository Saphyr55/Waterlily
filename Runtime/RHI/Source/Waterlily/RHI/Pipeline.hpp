#pragma once

namespace Wl
{

    class RHIPipeline
    {
    public:
        enum class Type
        {
            Graphics,
            Compute,
        };

        virtual Type GetType() = 0;

        virtual ~RHIPipeline() = default;
    };

}// namespace Wl