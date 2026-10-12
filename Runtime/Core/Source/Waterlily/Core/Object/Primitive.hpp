#pragma once

#include "Waterlily/Core/Object/MetaTable.hpp"

namespace Wl
{

    inline static const TypedType<void> VoidType = MetaTable::TypeOf<void>();
    inline static const TypedType<bool> BoolType = MetaTable::TypeOf<bool>();
    inline static const TypedType<char> CharType = MetaTable::TypeOf<char>();

    inline static const TypedType<int8> Int8Type = MetaTable::TypeOf<int8>();
    inline static const TypedType<int16> Int16Type = MetaTable::TypeOf<int16>();
    inline static const TypedType<int32> Int32Type = MetaTable::TypeOf<int32>();
    inline static const TypedType<int64> Int64Type = MetaTable::TypeOf<int64>();

    inline static const TypedType<uint8> UInt8Type = MetaTable::TypeOf<uint8>();
    inline static const TypedType<uint16> UInt16Type = MetaTable::TypeOf<uint16>();
    inline static const TypedType<uint32> UInt32Type = MetaTable::TypeOf<uint32>();
    inline static const TypedType<uint64> UInt64Type = MetaTable::TypeOf<uint64>();

    inline static const TypedType<float> FloatType = MetaTable::TypeOf<float>();
    inline static const TypedType<double> DoubleType = MetaTable::TypeOf<double>();

}// namespace Wl