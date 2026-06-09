#pragma once

#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <cstdarg>

namespace Wl
{

    template<typename... Args>
    inline String Format(StringRef format, Args&&... args)
    {
        int32_t size = ::snprintf(nullptr, 0, format, std::forward<Args>(args)...);

        if (size < 0)
        {
            return "";
        }

        String buffer;
        buffer.Resize(size);
        ::snprintf(buffer.GetData(), buffer.GetSize() + 1, format, std::forward<Args>(args)...);

        return buffer;
    }

    inline String Formatv(StringRef format, va_list args)
    {
        va_list argsCopy;
        va_copy(argsCopy, args);

        int size = ::vsnprintf(nullptr, 0, format, argsCopy);
        va_end(argsCopy);

        if (size <= 0)
        {
            return "";
        }

        String buffer;
        buffer.Resize(static_cast<size_t>(size));
        ::vsnprintf(buffer.data(), buffer.GetSize() + 1, format, args);

        return buffer;
    }

}// namespace Wl
