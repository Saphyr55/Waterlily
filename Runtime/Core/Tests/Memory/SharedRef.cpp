#include <catch2/catch_all.hpp>

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

using namespace Wl;

struct Base
{
    uint32_t Value = 0;
};

struct Derived : Base
{
};

TEST_CASE("Construct correctely.", "[SharedPtr::SharedPtr]")
{
    SharedPtr<uint32_t> ptr = MakeShared<uint32_t>(9);
    REQUIRE(ptr.IsValid());
    REQUIRE(ptr);
    REQUIRE(ptr.GetSharedReferenceCount() == 1);
    REQUIRE(*ptr == 9);

    ptr = MakeShared<uint32_t>(2);
    REQUIRE(ptr.IsValid());
    REQUIRE(ptr);
    REQUIRE(ptr.GetSharedReferenceCount() == 1);
    REQUIRE(*ptr == 2);

    SharedPtr<uint32_t> ptr2 = ptr;
    REQUIRE(ptr2.IsValid());
    REQUIRE(ptr2);
    REQUIRE(ptr2.GetSharedReferenceCount() == 2);// X
    REQUIRE(*ptr2 == 2);

    {
        SharedPtr<uint32_t> ptr3 = ptr;
        REQUIRE(ptr3.IsValid());
        REQUIRE(ptr3);
        REQUIRE(ptr.GetSharedReferenceCount() == 3);
        REQUIRE(ptr2.GetSharedReferenceCount() == 3);
        REQUIRE(ptr3.GetSharedReferenceCount() == 3);
        REQUIRE(*ptr3 == 2);
    }

    REQUIRE(ptr.GetSharedReferenceCount() == 2);
    REQUIRE(ptr2.GetSharedReferenceCount() == 2);
}

TEST_CASE("Construct correctely devired from.", "[SharedPtr::SharedPtr]")
{
    SharedPtr<Base> ptr = MakeShared<Derived>();
    REQUIRE(ptr.IsValid());
    REQUIRE(ptr);
    REQUIRE(ptr.GetSharedReferenceCount() == 1);

    ptr = MakeShared<Derived>();
    REQUIRE(ptr.IsValid());
    REQUIRE(ptr);
    REQUIRE(ptr.GetSharedReferenceCount() == 1);

    SharedPtr<Base> ptr2 = ptr;
    REQUIRE(ptr2.IsValid());
    REQUIRE(ptr2);
    REQUIRE(ptr2.GetSharedReferenceCount() == 2);// X

    {
        SharedPtr<Base> ptr3 = ptr;
        REQUIRE(ptr3.IsValid());
        REQUIRE(ptr3);
        REQUIRE(ptr.GetSharedReferenceCount() == 3);
        REQUIRE(ptr2.GetSharedReferenceCount() == 3);
        REQUIRE(ptr3.GetSharedReferenceCount() == 3);
    }

    REQUIRE(ptr.GetSharedReferenceCount() == 2);
    REQUIRE(ptr2.GetSharedReferenceCount() == 2);
}

TEST_CASE("Construct correctely in an Array.", "[SharedPtr::SharedPtr]")
{
    Array<SharedPtr<Base>> bases;
    bases.Resize(5);

    for (SharedPtr<Base> base: bases)
    {
        REQUIRE(!base.IsValid());
    }

    Array<SharedPtr<Base>> bases2;
    SharedPtr<Base> pbase = MakeShared<Base>(2);
    bases2.Resize(5, pbase);

    for (SharedPtr<Base> base: bases2)
    {
        REQUIRE(base.IsValid());
        REQUIRE(base->Value == 2);
    }
}
