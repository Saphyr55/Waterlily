#include "Waterlily/Core/Serialization/JSONArchive.hpp"

#include <catch2/catch_all.hpp>

using namespace Wl;

TEST_CASE("JSON archive round trip")
{
    JSONOutputArchive output;

    output.BeginObject("player");
    {
        output.Write<int64_t>("health", 100);
        output.Write<bool>("alive", true);
        output.BeginArray("inventory");
        {
            output.Write<int64_t>(StringID(), 1);
            output.Write<int64_t>(StringID(), 2);
            output.Write<int64_t>(StringID(), 3);
        }
        output.EndArray();
    }
    output.EndObject();

    JSONInputArchive input(output.GetJson());

    REQUIRE(input.BeginObject("player"));

    int64_t health = 0;
    bool alive = false;

    REQUIRE(input.Read<int64_t>("health", health));
    REQUIRE(input.Read<bool>("alive", alive));

    REQUIRE(input.BeginArray("inventory") == 3);
    {
        int64_t item1 = 0;
        int64_t item2 = 0;
        int64_t item3 = 0;

        REQUIRE(input.Read<int64_t>(StringID(), item1));
        REQUIRE(input.Read<int64_t>(StringID(), item2));
        REQUIRE(input.Read<int64_t>(StringID(), item3));

        CHECK(item1 == 1);
        CHECK(item2 == 2);
        CHECK(item3 == 3);
    }

    CHECK(health == 100);
    CHECK(alive);

    input.EndObject();
}

TEST_CASE("JSON archive output")
{
    JSONOutputArchive archive;

    archive.BeginObject("player");
    archive.Write<int64_t>("health", 100);
    archive.Write<bool>("alive", true);
    archive.EndObject();

    CHECK(archive.GetJson() ==
          JSONOutputArchive::Json {
                  {
                   "player",
                   {
                                  {"health", 100},
                                  {"alive", true},
                          },
                   },
    });
}