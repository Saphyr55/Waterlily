#include <catch2/catch_all.hpp>

#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

using namespace Wl;

struct ArrayCounterTest
{
    static int32_t Constructions;
    static int32_t Destructions;
    int32_t Value;

    ArrayCounterTest(int32_t v = 0)
        : Value(v)
    {
        Constructions++;
    }

    ArrayCounterTest(const ArrayCounterTest& other)
        : Value(other.Value)
    {
        Constructions++;
    }

    ArrayCounterTest& operator=(const ArrayCounterTest& other)
    {
        Value = other.Value;
        return *this;
    }

    ~ArrayCounterTest()
    {
        Destructions++;
    }

    bool operator==(const ArrayCounterTest& other) const
    {
        return Value == other.Value;
    }
};

int32_t ArrayCounterTest::Constructions = 0;
int32_t ArrayCounterTest::Destructions = 0;

TEST_CASE("Check Append, emplace, pop and indexing behaviours.", "[Array::Append]")
{
    Array<int32_t> a;
    REQUIRE(a.IsEmpty());
    REQUIRE(a.GetCapacity() >= 1);

    a.Append(10);
    a.Append(20);
    a.Emplace(30);
    REQUIRE(a.GetSize() == 3);
    REQUIRE(a[0] == 10);
    REQUIRE(a[1] == 20);
    REQUIRE(a[2] == 30);

    REQUIRE(a.front() == 10);
    REQUIRE(a.back() == 30);

    a.Pop();
    REQUIRE(a.GetSize() == 2);
    REQUIRE(a.back() == 20);

    a.Pop();
    a.Pop();
    REQUIRE(a.IsEmpty());
}

TEST_CASE("AppendRange copies contiguous ranges correctly.", "[Array::AppendRange]")
{
    Array<int32_t> a;
    for (int32_t i = 0; i < 10; i++)
    {
        a.Append(i);
    }

    Array<int32_t> b;
    b.AppendRange(a.begin(), a.end());

    REQUIRE(b.GetSize() == a.GetSize());

    for (size_t i = 0; i < a.GetSize(); i++)
    {
        REQUIRE(b[i] == a[i]);
    }
}

TEST_CASE("AppendRange works with struct types.", "[Array::AppendRange]")
{
    Array<ArrayCounterTest> a;

    a.Append(ArrayCounterTest(1));
    a.Append(ArrayCounterTest(2));
    a.Emplace(3);

    Array<ArrayCounterTest> b;
    b.AppendRange(a.begin(), a.end());

    REQUIRE(b.GetSize() == 3);

    REQUIRE(b[0].Value == 1);
    REQUIRE(b[1].Value == 2);
    REQUIRE(b[2].Value == 3);

    b[1].Value = 99;
    REQUIRE(b[1].Value == 99);
}

TEST_CASE("AppendRange does not leak or double destroy.", "[Array::AppendRange]")
{
    ArrayCounterTest::Constructions = 0;
    ArrayCounterTest::Destructions = 0;

    {
        Array<ArrayCounterTest> a;
        for (int i = 0; i < 5; i++)
        {
            a.Emplace(i);
        }

        Array<ArrayCounterTest> b;
        b.AppendRange(a.begin(), a.end());

        REQUIRE(b.GetSize() == 5);
    }

    REQUIRE(ArrayCounterTest::Constructions == ArrayCounterTest::Destructions);
}

TEST_CASE("Reserve, Resize and Shrink behaviour.", "[Array::reserve]")
{
    Array<int32_t> a;
    a.Reserve(50);
    REQUIRE(a.GetCapacity() >= 50);

    a.Resize(10);
    REQUIRE(a.GetSize() == 10);
    REQUIRE(a.GetCapacity() >= 50);

    a.Shrink();
    REQUIRE(a.GetCapacity() == a.GetSize());
}

TEST_CASE("Remove, Contains and getIf.", "[Array::Remove]")
{
    Array<int32_t> a = {1, 2, 3, 4, 5, 3, 6};

    REQUIRE(a.Contains(3));
    a.Remove(3);
    REQUIRE_FALSE(a.Contains(3));
    REQUIRE(a.GetSize() == 5);

    auto p = a.GetIf([](int32_t v)
    {
        return v % 2 == 0;
    });// first even
    REQUIRE(p != nullptr);
    REQUIRE(*p % 2 == 0);

    a.RemoveIf([](auto& x) -> bool
    {
        return x == 4;
    });
    REQUIRE_FALSE(a.Contains(4));
}

TEST_CASE("Copy, move, appendRange and initializer list.", "[Array::Array]")
{
    Array<int32_t> a = {5, 6, 7};
    Array<int32_t> b(a);
    REQUIRE(a == b);

    Array<int32_t> c;
    c.AppendRange(a);
    REQUIRE(c == a);

    Array<int32_t> d(std::move(a));
    REQUIRE(d.GetSize() == 3);

    Array<int32_t> e;
    e = b;
    REQUIRE(e == b);

    Array<int32_t> f;
    f = std::move(b);
    REQUIRE(f.GetSize() == 3);
}

TEST_CASE("Transform behaviour.", "[transform]")
{
    Array<int32_t> a = {1, 2, 3, 4};

    Array<double> mapped;
    mapped.Resize(a.GetSize());

    Wl::Transform(a.begin(), a.end(), mapped.begin(), [](int32_t v) -> double
    {
        return static_cast<double>(v) / 2.0;
    });

    REQUIRE(mapped.GetSize() == a.GetSize());
    for (size_t i = 0; i < mapped.GetSize(); i++)
    {
        REQUIRE(mapped[i] == Catch::Approx(static_cast<double>(a[i]) / 2.0));
    }

    Array<ArrayCounterTest> mapped2;
    mapped2.Resize(a.GetSize());
    Wl::Transform(a.begin(), a.end(), mapped2.begin(), [](int32_t v) -> ArrayCounterTest
    {
        return ArrayCounterTest(v * 10);
    });

    REQUIRE(mapped2.GetSize() == a.GetSize());
    for (size_t i = 0; i < mapped2.GetSize(); i++)
    {
        REQUIRE(mapped2[i].Value == a[i] * 10);
    }
}

TEST_CASE("Iterators and range.", "[Array::iterators]")
{
    Array<int32_t> a;
    for (int32_t i = 0; i < 10; i++)
    {
        a.Append(i);
    }

    int32_t sum = 0;
    for (int32_t v: a)
    {
        sum += v;
    }
    REQUIRE(sum == 45);// 0..9 sum
}

TEST_CASE("Equality, inequality and swap.", "[Array::operators]")
{
    Array<int32_t> a = {1, 2, 3};
    Array<int32_t> b = {1, 2, 3};
    Array<int32_t> c = {3, 2, 1};

    REQUIRE(a == b);
    REQUIRE(a != c);

    a.Swap(c);
    REQUIRE(a[0] == 3);
    REQUIRE(c[0] == 1);
}

TEST_CASE("Allocator, destruction check via ArrayCounterTest.", "[Array::destruction]")
{
    ArrayCounterTest::Constructions = 0;
    ArrayCounterTest::Destructions = 0;

    {
        Array<ArrayCounterTest> arr;
        arr.Append(ArrayCounterTest(10));
        arr.Append(ArrayCounterTest(20));
        arr.Emplace(30);
        REQUIRE(arr.GetSize() == 3);
    }

    REQUIRE(ArrayCounterTest::Constructions == ArrayCounterTest::Destructions);
}

TEST_CASE("reallocate moves elements and preserves values", "[Array::reallocate]")
{
    Array<std::string> a;
    for (int32_t i = 0; i < 8; i++)
    {
        a.Append(std::string("s") + std::to_string(i));
    }

    auto old_values = a;
    a.Reserve(32);
    REQUIRE(a.GetSize() == old_values.GetSize());
    REQUIRE(a == old_values);
}

TEST_CASE("Nested Array<Array<T>> behavior", "[Array::Array]")
{
    ArrayCounterTest::Constructions = 0;
    ArrayCounterTest::Destructions = 0;

    {
        Array<Array<ArrayCounterTest>> nested;// Array of Arrays

        // Create a few inner arrays
        Array<ArrayCounterTest> inner1;
        inner1.Append(ArrayCounterTest(1));
        inner1.Append(ArrayCounterTest(2));

        Array<ArrayCounterTest> inner2;
        inner2.Append(ArrayCounterTest(10));
        inner2.Append(ArrayCounterTest(20));
        inner2.Append(ArrayCounterTest(30));

        // Append inner arrays into the outer one
        nested.Append(inner1);
        nested.Append(std::move(inner2));

        REQUIRE(nested.GetSize() == 2);
        REQUIRE(nested[0].GetSize() == 2);
        REQUIRE(nested[1].GetSize() == 3);

        // Check values
        REQUIRE(nested[0][0].Value == 1);
        REQUIRE(nested[0][1].Value == 2);
        REQUIRE(nested[1][0].Value == 10);
        REQUIRE(nested[1][2].Value == 30);

        // Modify nested elements to ensure deep access works
        nested[0][1].Value = 99;
        REQUIRE(nested[0][1].Value == 99);

        // Copy the whole nested array
        Array<Array<ArrayCounterTest>> copy = nested;
        REQUIRE(copy.GetSize() == nested.GetSize());
        REQUIRE(copy[0][1].Value == 99);

        // Move it to another
        Array<Array<ArrayCounterTest>> moved = std::move(copy);
        REQUIRE(moved.GetSize() == 2);
        REQUIRE(moved[0][1].Value == 99);
    }

    // Ensure all constructed elements were properly destroyed
    REQUIRE(ArrayCounterTest::Constructions == ArrayCounterTest::Destructions);
}

TEST_CASE("move_data and move assignment corner cases", "[Array::move]")
{
    Array<int32_t> src = {9, 8, 7};
    Array<int32_t> target(0);
    target = src;
    REQUIRE(target.GetSize() == src.GetSize());
    REQUIRE(target == src);

    Array<int32_t> m = std::move(target);
    REQUIRE(m.GetSize() == 3);
}
