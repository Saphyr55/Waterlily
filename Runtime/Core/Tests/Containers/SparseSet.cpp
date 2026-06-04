#include <catch2/catch_all.hpp>

#include "Waterlily/Core/Containers/SparseSet.hpp"
#include "Waterlily/Core/String/String.hpp"

using namespace Wl;

struct SparseSetCounterTest
{
    static int32_t constructions;
    static int32_t destructions;
    int32_t value;

    SparseSetCounterTest(int32_t v = 0)
        : value(v)
    {
        ++constructions;
    }

    SparseSetCounterTest(const SparseSetCounterTest& o)
        : value(o.value)
    {
        ++constructions;
    }

    SparseSetCounterTest& operator=(const SparseSetCounterTest& o)
    {
        value = o.value;
        return *this;
    }

    ~SparseSetCounterTest()
    {
        ++destructions;
    }

    bool operator==(const SparseSetCounterTest& o) const
    {
        return value == o.value;
    }
};

int32_t SparseSetCounterTest::constructions = 0;
int32_t SparseSetCounterTest::destructions = 0;

TEST_CASE("Basic Put, get, and Remove.", "[SparseSet]")
{
    SparseSet<int32_t, uint32_t> set;

    REQUIRE(set.IsEmpty());

    set.Put(10, 100);
    set.Put(20, 200);
    set.Put(30, 300);

    REQUIRE(set.GetSize() == 3);
    REQUIRE(set.Contains(10));
    REQUIRE(set.Contains(20));
    REQUIRE(set.Contains(30));

    REQUIRE(set[10] == 100);
    REQUIRE(set[20] == 200);
    REQUIRE(set[30] == 300);

    set.Put(20, 999);
    REQUIRE(set[20] == 999);

    set.Remove(20);
    REQUIRE_FALSE(set.Contains(20));
    REQUIRE(set.GetSize() == 2);

    set.Remove(10);
    REQUIRE(set.GetSize() == 1);
    REQUIRE(set.Contains(30));
}

TEST_CASE("Reserve, Resize, and Clear.", "[SparseSet]")
{
    SparseSet<int32_t, uint32_t> set(0);

    set.Reserve(100);
    REQUIRE(set.IsEmpty());

    set.Put(1, 11);
    set.Put(2, 22);
    set.Put(3, 33);
    REQUIRE(set.GetSize() == 3);

    set.Resize(5);
    REQUIRE(set.GetSize() == 5);

    set.Clear();
    REQUIRE(set.IsEmpty());
}

TEST_CASE("Contains and get pointer interface", "[SparseSet]")
{
    SparseSet<String, uint32_t> set;

    set.Put(5, "apple");
    set.Put(8, "banana");

    String* p1 = set.Get(5);
    String* p2 = set.Get(99);

    REQUIRE(p1 != nullptr);
    REQUIRE(*p1 == "apple");
    REQUIRE(p2 == nullptr);
    REQUIRE(set.Contains(8));
    REQUIRE_FALSE(set.Contains(42));
}

TEST_CASE("Dense array integrity and stable iteration", "[SparseSet::elements]")
{
    SparseSet<int32_t, uint32_t> set;
    for (int i = 0; i < 10; i++)
    {
        set.Put(i, i * 10);
    }

    const auto& dense = set.GetElements();
    REQUIRE(dense.GetSize() == 10);

    // Values in dense array correspond to inserted values
    for (int i = 0; i < 10; i++)
    {
        REQUIRE(set.Contains(i));
        REQUIRE(set[i] == i * 10);
    }

    // Removing middle element doesn't invalidate others
    set.Remove(5);
    REQUIRE_FALSE(set.Contains(5));
    REQUIRE(set.GetSize() == 9);
}

TEST_CASE("Destruction and lifecycle tracking.", "[SparseSet::destruction]")
{
    SparseSetCounterTest::constructions = 0;
    SparseSetCounterTest::destructions = 0;

    {
        SparseSet<SparseSetCounterTest, uint32_t> set;
        set.Put(1, SparseSetCounterTest(10));
        set.Put(2, SparseSetCounterTest(20));
        set.Put(3, SparseSetCounterTest(30));
        REQUIRE(set.GetSize() == 3);

        set.Remove(2);
        REQUIRE(set.GetSize() == 2);
    }

    REQUIRE(SparseSetCounterTest::constructions == SparseSetCounterTest::destructions);
}

TEST_CASE("Paged indexing and large sparse indices.", "[SparseSet::paging]")
{
    SparseSet<int32_t, uint32_t> set(8);

    set.Put(0, 10);
    set.Put(15, 150);
    set.Put(63, 630);
    set.Put(128, 1280);

    REQUIRE(set.Contains(0));
    REQUIRE(set.Contains(15));
    REQUIRE(set.Contains(63));
    REQUIRE(set.Contains(128));

    REQUIRE(set[15] == 150);
    REQUIRE(set[63] == 630);
    REQUIRE(set[128] == 1280);
}

TEST_CASE("Clear resets all sparse pages and dense data.", "[SparseSet::Clear]")
{
    SparseSet<int32_t, uint32_t> set;
    for (int i = 0; i < 20; i++)
    {
        set.Put(i, i);
    }

    REQUIRE(set.GetSize() == 20);
    REQUIRE_FALSE(set.IsEmpty());

    set.Clear();
    REQUIRE(set.IsEmpty());
    REQUIRE_FALSE(set.Contains(10));
}
