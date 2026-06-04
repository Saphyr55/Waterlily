#pragma once

#include "LauncherExports.hpp"
#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    class Application;

    using EngineAppCallback = int32_t(Application&);

    WL_LAUNCHER_API int32_t Main(int32_t argc, const char** argv, EngineAppCallback* func);

}// namespace Wl
