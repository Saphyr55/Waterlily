#include "Waterlily/Launcher/CommandParser.hpp"

#include "Waterlily/Core/IO/PlatformFileSystem.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    HashMap<StringRef, StringRef> CommandLineParse(int32_t argc, const char** argv)
    {
        StringRef projectdir = PlatformFileSystem::get_current_directory();
        StringRef enginedir = PlatformFileSystem::get_current_directory();

        for (uint8_t i = 1; i < argc; i++)
        {
            StringRef arg = argv[i];

            if (arg == "--projectdir")
            {
                if (i + 1 >= argc)
                {
                    LLOG_ERROR("[main]", "--projectdir requires an argument.");
                    return false;
                }
                projectdir = argv[++i];
                LLOG_INFO("[main]", Wl::Format("Set project directory to: '%s'", projectdir.data()));
                continue;
            }

            if (arg == "--enginedir")
            {
                if (i + 1 >= argc)
                {
                    LLOG_ERROR("[main]", "--enginedir requires an argument.");
                    return false;
                }
                enginedir = argv[++i];
                LLOG_INFO("[main]", Wl::Format("Set engine directory to: '%s'", enginedir.data()));
                continue;
            }
        }

        return {
                {"projectdir", projectdir},
                {"enginedir", enginedir},
        };
    }

}// namespace Wl