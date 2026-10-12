#pragma once

#include "Waterlily/Launcher/LauncherExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Function/Function.hpp"

namespace Wl
{

    using MainConsoleCallback = Function<int32()>;

    WL_LAUNCHER_API int32 MainConsole(int32 argc, const char* argv[], MainConsoleCallback callback);

    WL_LAUNCHER_API int32 MainApplication(int32 argc, const char* argv[]);

}// namespace Wl
