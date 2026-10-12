#pragma once

#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Variant.hpp"
#include "Waterlily/Core/Serialization/InputArchive.hpp"
#include "Waterlily/Core/Serialization/OutputArchive.hpp"

namespace Wl
{

    void Serialize(OutputArchive& ar, StringRef name, const String& value);
    void Deserialize(InputArchive& ar, StringRef name, String& value);

    template<typename T>
    void Serialize(OutputArchive& ar, StringRef name, const Array<T>& array)
    {
        ar.BeginArray(name, array.size());
        for (const T& element: array)
        {
            Serialize(ar, "", element);
        }
        ar.EndArray();
    }

    template<typename T>
    void Deserialize(InputArchive& ar, StringRef name, Array<T>& array)
    {
        usize size = ar.BeginArray(name);
        array.Reserve(size);
        for (usize i = 0; i < size; i++)
        {

        }
        ar.EndArray();
    }

}// namespace Wl