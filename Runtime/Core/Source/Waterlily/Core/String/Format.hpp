#pragma once

#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    template<typename... Args>
    inline String Format(StringRef fmt, Args&&... args)
    {
        int32_t size = ::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);

        if (size < 0)
        {
            return "";
        }

        String buffer;
        buffer.Resize(size);
        ::snprintf(buffer.GetData(), buffer.GetSize() + 1, fmt, std::forward<Args>(args)...);

        return buffer;
    }

}// namespace Wl
