#include "Types.hpp"

void Bar::_RegisterBindings()
{
}

void Foo::_RegisterBindings()
{
    MetaTable::RegisterMember("x", &Foo::m_x);
    MetaTable::RegisterMember("y", &Foo::m_y);
    MetaTable::RegisterMember("z", &Foo::z);

    MetaTable::RegisterMethod("FooMethod", &Foo::FooMethod);
    MetaTable::RegisterMethod("GetRefX", &Foo::GetRefX);

    MetaTable::RegisterMethod("GetX", &Foo::GetX);
    MetaTable::RegisterMethod("SetX", &Foo::SetX);

    MetaTable::RegisterProperty(Foo::StaticType(), "x", "GetX", "SetX");
}
