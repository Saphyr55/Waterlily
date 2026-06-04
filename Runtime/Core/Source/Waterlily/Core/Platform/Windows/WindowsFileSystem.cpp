#ifdef _WIN32
#include "Waterlily/Core/Platform/Windows/Windows.hpp"

#include "Waterlily/Core/Platform/PlatformFileSystem.hpp"
#include "Waterlily/Core/String/String.hpp"

namespace Wl
{

    const char* PlatformGetCurrentDirectory()
    {
        static char buffer[1024];
        DWORD ret = GetCurrentDirectoryA(MAX_PATH, buffer);
        if (ret == 0 || ret >= MAX_PATH)
        {
            buffer[0] = '\0';
        }
        return buffer;
    }

}// namespace Wl

#endif