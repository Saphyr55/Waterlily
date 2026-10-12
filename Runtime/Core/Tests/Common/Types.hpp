#pragma once

#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/String/Format.hpp"

using namespace Wl;

class Bar : public Object
{
    WL_OBJECT(Bar, Object);
};

class Foo : public Bar
{
    WL_OBJECT(Foo, Bar);

public:
    String FooMethod(int x, float r) const
    {
        return Format("%.1f", (x + m_y) / r);
    }

    int& GetRefX()
    {
        return m_x;
    }

    int GetX() const
    {
        return m_x;
    }

    void SetX(int x)
    {
        m_x = x;
    }

public:
    Foo() = default;
    Foo(int y)
        : m_y(y)
    {
    }

private:
    int m_x = 91;
    const int m_y = 0;

public:
    String z;

public:
    static usize OffsetOfX()
    {
        return offsetof(Foo, m_x);
    }

    static usize OffsetOfY()
    {
        return offsetof(Foo, m_y);
    }

    static usize OffsetOfZ()
    {
        return offsetof(Foo, z);
    }
};
