#include "Waterlily/Core/Serialization/Json/JsonOutputArchive.hpp"
#include "Waterlily/Core/Serialization/Json/JsonInputArchive.hpp"

#include <catch2/catch_all.hpp>

using namespace Wl;

TEST_CASE("JSON archive round trip")
{
    JsonOutputArchive output;

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

    JsonInputArchive input(output.GetJson());

    REQUIRE(input.BeginObject("player"));

    int32 health = 0;
    bool alive = false;

    REQUIRE(input.Read("health", health));
    REQUIRE(input.Read("alive", alive));

    REQUIRE(input.BeginArray("inventory") == 3);
    {
        int32 item1 = 0;
        int32 item2 = 0;
        int32 item3 = 0;

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
    JsonOutputArchive archive;

    archive.BeginObject("player");
    archive.Write("health", 100);
    archive.Write("alive", true);
    archive.EndObject();

    CHECK(archive.GetJson() ==
          JsonOutputArchive::Json {
                  {
                   "player",
                   {
                                  {"health", 100},
                                  {"alive", true},
                          },
                   },
    });
}