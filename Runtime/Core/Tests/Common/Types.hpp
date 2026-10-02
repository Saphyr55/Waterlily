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

private:
    int m_x = 91;
    const int m_y = 0;

public:
    String z;

public:
    static size_t OffsetOfX()
    {
        return offsetof(Foo, m_x);
    }

    static size_t OffsetOfY()
    {
        return offsetof(Foo, m_y);
    }

    static size_t OffsetOfZ()
    {
        return offsetof(Foo, z);
    }
};
