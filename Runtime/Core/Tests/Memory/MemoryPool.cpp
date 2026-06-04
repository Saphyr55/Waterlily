#include "Waterlily/Core/Memory/MemoryPool.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/String/String.hpp"

#include <catch2/catch_all.hpp>

using namespace Wl;

struct TestObject
{
    String Name = "TestName";
    int Value = -1;

    TestObject() = default;
    TestObject(const String& n, int v)
        : Name(n)
        , Value(v)
    {
    }

    ~TestObject()
    {
        Value = 0;
        Name = "";
    }
};

TEST_CASE("MemoryPool basic allocation and deallocation.", "[MemoryPool]")
{
    MemoryPool<TestObject> pool;
    pool.InitPool(&DefaultAllocator::GetInstance(), 4);

    SECTION("Allocate and deallocate resources")
    {
        TestObject* obj1 = pool.Allocate();
        TestObject* obj2 = pool.Allocate();

        REQUIRE(obj1 != nullptr);
        REQUIRE(obj2 != nullptr);
        REQUIRE(obj1 != obj2);

        pool.Deallocate(obj1);
        pool.Deallocate(obj2);

        TestObject* obj3 = pool.Allocate();
        TestObject* obj4 = pool.Allocate();

        REQUIRE((obj3 == obj2 || obj3 == obj1));
        REQUIRE((obj4 == obj1 || obj4 == obj2));
    }

    SECTION("Construct and delete resources.")
    {
        TestObject* obj = pool.NewResource("ConstructTest", 42);
        REQUIRE(obj->Value == 42);
        REQUIRE(obj->Name == "ConstructTest");

        pool.DeleteResource(obj);
        // undefined behavior: the union is used to reduce the overhead of the linked list.
    }

    SECTION("Pool grows when exhausted")
    {
        Array<TestObject*> objs(10);
        for (int i = 0; i < 10; i++)
        {
            objs.Append(pool.NewResource("Name", i));
        }

        for (int i = 0; i < 10; i++)
        {
            REQUIRE(objs[i]->Value == i);
            REQUIRE(objs[i]->Name == "Name");
        }

        for (auto* obj: objs)
        {
            pool.DeleteResource(obj);
        }
    }

    pool.Dispose();
}
