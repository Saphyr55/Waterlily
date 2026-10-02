#pragma once

#include <cstdint>

namespace Wl
{

    enum class SRGUpdateFrequency : uint8
    {
        PerFrame,
        PerPass,
        PerGroup,
        OneTime,
    };

}