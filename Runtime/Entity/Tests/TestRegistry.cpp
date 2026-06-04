#include "Waterlily/Entity/Registry.hpp"
#include <catch2/catch_all.hpp>

using namespace Wl;

struct Position
{
    float x, y;
};

struct Velocity
{
    float dx, dy;
};

struct Health
{
    int hp;
};

TEST_CASE("Entity creation and validity.", "[EntityRegistry]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    REQUIRE(e1 != e2);
    REQUIRE(registry.IsValid(e1));
    REQUIRE(registry.IsValid(e2));

    registry.Destroy(e1);
    REQUIRE_FALSE(registry.IsValid(e1));
    REQUIRE(registry.IsValid(e2));
}

TEST_CASE("Add, get, and remove components.", "[EntityRegistry]")
{
    EntityRegistry registry;
    Entity e = registry.Create();

    REQUIRE_FALSE(registry.HasComponent<Position>(e));
    REQUIRE(registry.GetComponent<Position>(e) == nullptr);

    Position position{1.0f, 2.0f};
    registry.AddComponent<Position>(e, position);

    REQUIRE(registry.HasComponent<Position>(e));
    auto* positionPtr = registry.GetComponent<Position>(e);
    REQUIRE(positionPtr != nullptr);
    REQUIRE(positionPtr->x == 1.0f);
    REQUIRE(positionPtr->y == 2.0f);

    registry.RemoveComponent<Position>(e);
    REQUIRE_FALSE(registry.HasComponent<Position>(e));
    REQUIRE(registry.GetComponent<Position>(e) == nullptr);
}

TEST_CASE("Multiple component types.", "[EntityRegistry]")
{
    EntityRegistry registry;
    Entity e = registry.Create();

    registry.AddComponent<Position>(e, {10, 20});
    registry.AddComponent<Velocity>(e, {1, 2});

    REQUIRE(registry.HasComponent<Position>(e));
    REQUIRE(registry.HasComponent<Velocity>(e));

    Position* pos = registry.GetComponent<Position>(e);
    Velocity* vel = registry.GetComponent<Velocity>(e);

    REQUIRE(pos->x == 10);
    REQUIRE(pos->y == 20);
    REQUIRE(vel->dx == 1);
    REQUIRE(vel->dy == 2);
}

TEST_CASE("Iterate over single components.", "[EntityRegistry]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();
    Entity e3 = registry.Create();

    registry.AddComponent<Position>(e1, {1, 1});
    registry.AddComponent<Position>(e2, {2, 2});
    registry.AddComponent<Position>(e3, {3, 3});

    int32_t sumX = 0, sumY = 0;
    registry.ForEach<Position>([&](Entity e, Position& p) -> void
    {
        sumX += static_cast<int>(p.x);
        sumY += static_cast<int>(p.y);
    });

    REQUIRE(sumX == 6);
    REQUIRE(sumY == 6);
}

TEST_CASE("Iterate over multiple components.", "[EntityRegistry]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();
    Entity e3 = registry.Create();

    registry.AddComponent<Position>(e1, {1, 1});
    registry.AddComponent<Velocity>(e1, {10, 20});

    registry.AddComponent<Position>(e2, {2, 2});
    // e2 has no velocity

    registry.AddComponent<Position>(e3, {3, 3});
    registry.AddComponent<Velocity>(e3, {30, 40});

    float sumX = 0, sumdx = 0;
    registry.ForEach<Position, Velocity>([&](Entity e, Position& p, Velocity& v) -> void
    {
        sumX += static_cast<float>(p.x);
        sumdx += static_cast<float>(v.dx);
    });

    // Only e1 and e3 should be counted
    REQUIRE(sumX == 1 + 3);
    REQUIRE(sumdx == 10 + 30);
}

TEST_CASE("Destroy entities removes components.", "[EntityRegistry]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    registry.AddComponent<Position>(e1, {1, 1});
    registry.AddComponent<Position>(e2, {2, 2});

    registry.Destroy(e1);

    REQUIRE_FALSE(registry.IsValid(e1));
    REQUIRE(registry.IsValid(e2));

    REQUIRE(registry.GetComponent<Position>(e1) == nullptr);
    REQUIRE(registry.GetComponent<Position>(e2) != nullptr);
}

TEST_CASE("Disposing registry removes all entities and components.", "[EntityRegistry]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    registry.AddComponent<Position>(e1, {1, 1});
    registry.AddComponent<Velocity>(e2, {5, 5});

    registry.Dispose();

    REQUIRE_FALSE(registry.IsValid(e1));
    REQUIRE_FALSE(registry.IsValid(e2));

    REQUIRE(registry.GetComponent<Position>(e1) == nullptr);
    REQUIRE(registry.GetComponent<Velocity>(e2) == nullptr);
}
