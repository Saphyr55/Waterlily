#pragma once

#include "LauncherExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Engine/Application.hpp"

namespace Wl
{
    using MainConsoleCallback = int32_t();

    WL_LAUNCHER_API int32_t MainConsole(int32_t argc, const char* argv[], MainConsoleCallback* callback);
    
    WL_LAUNCHER_API int32_t MainApplication(int32_t argc, const char* argv[]);

}// namespace Wl
