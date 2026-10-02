#include "Waterlily/Core/Logging/ConsoleLoggerWriter.hpp"
#include "Waterlily/Core/Logging/Logger.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"

#include <catch2/catch_session.hpp>

int main(int argc, char* argv[])
{
    Wl::Logger::RegisterWriter(Wl::ConsoleLoggerWriter::Name, Wl::MakeShared<Wl::ConsoleLoggerWriter>());
    Wl::MetaTable::Init();

    int result = Catch::Session().run(argc, argv);

    Wl::MetaTable::Shutdown();
    Wl::Logger::UnregisterWriter(Wl::ConsoleLoggerWriter::Name);

    return result;
}