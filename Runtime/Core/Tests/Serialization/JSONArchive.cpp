#include "Waterlily/Core/Serialization/JSONArchive.hpp"

#include <catch2/catch_all.hpp>

using namespace Wl;

TEST_CASE("JSON archive round trip")
{
    JSONOutputArchive output;

    output.BeginObject("player");
    {
        output.Write("health", 100);
        output.Write("alive", true);
        output.BeginArray("inventory");
        {
            output.Write("", 1);
            output.Write("", 2);
            output.Write("", 3);
        }
        output.EndArray();
    }
    output.EndObject();

    JSONInputArchive input(output.GetJson());

    REQUIRE(input.BeginObject("player"));

    int32_t health = 0;
    bool alive = false;

    REQUIRE(input.Read("health", health));
    REQUIRE(input.Read("alive", alive));

    REQUIRE(input.BeginArray("inventory") == 3);
    {
        int32_t item1 = 0;
        int32_t item2 = 0;
        int32_t item3 = 0;

        REQUIRE(input.Read("", item1));
        REQUIRE(input.Read("", item2));
        REQUIRE(input.Read("", item3));

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
    archive.Write("health", 100);
    archive.Write("alive", true);
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