#include "Waterlily/Entity/EntityView.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"

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

TEST_CASE("View with single component.", "[EntityView]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();
    Entity e3 = registry.Create();

    registry.AddComponent<Position>(e1, {1.0f, 1.0f});
    registry.AddComponent<Position>(e2, {2.0f, 2.0f});
    registry.AddComponent<Velocity>(e3, {3.0f, 3.0f});

    SECTION("Basic view iteration")
    {
        float sum_x = 0.0f;
        auto view = registry.View<Position>();
        view.ForEach([&](Entity e, Position& pos)
        {
            sum_x += pos.x;
        });

        REQUIRE(sum_x == 3.0f);
    }

    SECTION("View size")
    {
        auto view = registry.View<Position>();
        REQUIRE(view.GetSize() == 2);
        REQUIRE_FALSE(view.IsEmpty());
    }
}

TEST_CASE("View with multiple components.", "[EntityView]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();
    Entity e3 = registry.Create();
    Entity e4 = registry.Create();

    registry.AddComponent<Position>(e1, {1.0f, 1.0f});
    registry.AddComponent<Velocity>(e1, {0.1f, 0.1f});
    registry.AddComponent<Health>(e1, {100});

    registry.AddComponent<Position>(e2, {2.0f, 2.0f});

    registry.AddComponent<Position>(e3, {3.0f, 3.0f});
    registry.AddComponent<Velocity>(e3, {0.3f, 0.3f});

    registry.AddComponent<Velocity>(e4, {0.4f, 0.4f});

    SECTION("Two components view")
    {
        int32_t count = 0;
        float sum_x = 0.0f, sum_dx = 0.0f;

        auto view = registry.View<Position, Velocity>();
        view.ForEach([&](Entity e, Position& pos, Velocity& vel) -> void
        {
            count++;
            sum_x += pos.x;
            sum_dx += vel.dx;
        });

        REQUIRE(count == 2);
        REQUIRE(sum_x == 4.0f);
        REQUIRE(sum_dx == 0.4f);
    }

    SECTION("Three components view")
    {
        int32_t count = 0;

        auto view = registry.View<Position, Velocity, Health>();
        view.ForEach([&](Entity e, Position& pos, Velocity& vel, Health& health) -> void
        {
            count++;
            REQUIRE(health.hp == 100);
        });

        REQUIRE(count == 1);
    }
}

TEST_CASE("View with range-based for loop.", "[EntityView]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    registry.AddComponent<Position>(e1, {1.0f, 2.0f});
    registry.AddComponent<Velocity>(e1, {3.0f, 4.0f});
    registry.AddComponent<Position>(e2, {5.0f, 6.0f});
    registry.AddComponent<Velocity>(e2, {7.0f, 8.0f});

    SECTION("Range-based iteration")
    {
        float sum_x = 0.0f;
        float sum_dx = 0.0f;
        int32_t count = 0;

        for (const auto [entity, pos, vel]: registry.View<Position, Velocity>())
        {
            count++;
            sum_x += pos.x;
            sum_dx += vel.dx;
        }

        REQUIRE(count == 2);
        REQUIRE(sum_x == 6.0f);
        REQUIRE(sum_dx == 10.0f);
    }
}

TEST_CASE("View with entity modification.", "[EntityView]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    registry.AddComponent<Position>(e1, {1.0f, 1.0f});
    registry.AddComponent<Velocity>(e1, {0.5f, 0.5f});
    registry.AddComponent<Position>(e2, {2.0f, 2.0f});
    registry.AddComponent<Velocity>(e2, {1.0f, 1.0f});

    SECTION("Modify components in view")
    {
        EntityView<Position, Velocity> view = registry.View<Position, Velocity>();
        view.ForEach([&](Entity e, Position& pos, Velocity& vel)
        {
            pos.x += vel.dx;
            pos.y += vel.dy;
        });

        REQUIRE(registry.GetComponent<Position>(e1)->x == 1.5f);
        REQUIRE(registry.GetComponent<Position>(e1)->y == 1.5f);
        REQUIRE(registry.GetComponent<Position>(e2)->x == 3.0f);
        REQUIRE(registry.GetComponent<Position>(e2)->y == 3.0f);
    }

    SECTION("Remove entities during iteration")
    {
        EntityView<Position> view = registry.View<Position>();
        int32_t count = 0;

        view.ForEach([&](Entity e, Position& pos) -> void
        {
            count++;
            if (e == e1)
            {
                registry.Destroy(e1);
            }
        });

        REQUIRE(count == 1);// The count is 1 because we destroyed e1.
        REQUIRE_FALSE(registry.IsValid(e1));
        REQUIRE(registry.IsValid(e2));
    }
}

TEST_CASE("Empty view.", "[EntityView]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    registry.AddComponent<Velocity>(e1, {1.0f, 1.0f});

    SECTION("View with no matching entities")
    {
        EntityView<Position> view = registry.View<Position>();
        REQUIRE(view.IsEmpty());

        int32_t count = 0;
        view.ForEach([&](Entity e, Position& pos) -> void
        {
            count++;
        });

        REQUIRE(count == 0);
    }

    SECTION("View with non-existent component")
    {
        EntityView<Position, Velocity> view = registry.View<Position, Velocity>();
        REQUIRE(view.GetSize() == 0);
    }
}

TEST_CASE("View after entity destruction.", "[EntityView]")
{
    EntityRegistry registry;

    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    registry.AddComponent<Position>(e1, {1.0f, 1.0f});
    registry.AddComponent<Position>(e2, {2.0f, 2.0f});

    EntityView<Position> view = registry.View<Position>();
    REQUIRE(view.GetSize() == 2);

    registry.Destroy(e1);

    REQUIRE(view.GetSize() == 1);

    int32_t count = 0;
    view.ForEach([&](Entity e, Position& pos) -> void
    {
        count++;
        REQUIRE(e == e2);
    });

    REQUIRE(count == 1);
}