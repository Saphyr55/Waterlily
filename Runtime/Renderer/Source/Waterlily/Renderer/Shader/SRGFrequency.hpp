#pragma once

#include <cstdint>

namespace Wl
{

    enum class SRGUpdateFrequency : uint8_t
    {
        PerFrame,
        PerPass,
        PerGroup,
        OneTime,
    };

}