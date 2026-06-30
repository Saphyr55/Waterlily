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

#define WL_DEFINE_ENTRY_POINT(WlMain, ApplicationCallback) \
    int32_t main(int32_t argc, const char** argv)          \
    {                                                      \
        return WlMain(argc, argv, ApplicationCallback);    \
    }

#define WL_DEFINE_CONSOLE_ENTRY_POINT(ApplicationCallback)      \
    WL_DEFINE_ENTRY_POINT(Wl::MainConsole, ApplicationCallback)

#define WL_DEFINE_APPLICATION_ENTRY_POINT(ApplicationCallback)      \
    WL_DEFINE_ENTRY_POINT(Wl::MainApplication, ApplicationCallback)
