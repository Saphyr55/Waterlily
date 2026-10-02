#pragma once

#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class InputArchive
    {
    public:
        virtual bool BeginObject(StringRef name) = 0;
        virtual void EndObject() = 0;

        virtual usize BeginArray(StringRef name) = 0;
        virtual void EndArray() = 0;

        virtual bool Read(StringRef name, void* value, Type type) = 0;

        virtual bool Read(StringRef name, Object& value)
        {
            return Read(name, &value, value.GetObjectType());
        }

        template<typename T>
        inline bool Read(StringRef name, T& value)
        {
            return Read(name, &value, MetaTable::TypeOf<T>());
        }

        virtual ~InputArchive() = default;
    };

}// namespace Wl