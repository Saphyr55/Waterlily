#pragma once

#include "LauncherExports.hpp"

#include <cstdint>

namespace Wl
{

    class Application;

    using EngineConsoleCallback = int32_t();
    using EngineApplicationCallback = int32_t(Application&);

    WL_LAUNCHER_API int32_t MainConsole(int32_t argc, const char** argv, EngineConsoleCallback* func);

    WL_LAUNCHER_API int32_t MainApplication(int32_t argc, const char** argv, EngineApplicationCallback* func);

}// namespace Wl
