#pragma once

#include "LauncherExports.hpp"
#include "Waterlily/Core/Function/Function.hpp"

#include <cstdint>

namespace Wl
{

    using MainConsoleCallback = Function<int32_t()>;

    WL_LAUNCHER_API int32_t MainConsole(int32_t argc, const char* argv[], MainConsoleCallback callback);

    WL_LAUNCHER_API int32_t MainApplication(int32_t argc, const char* argv[]);

}// namespace Wl
