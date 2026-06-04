#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Launcher/LauncherExports.hpp"

namespace Wl
{

    WL_LAUNCHER_API HashMap<StringRef, StringRef> CommandLineParse(int32_t argc, const char** argv);

}
