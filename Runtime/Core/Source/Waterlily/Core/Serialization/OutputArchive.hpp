#pragma once

#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class OutputArchive
    {
    public:
        virtual void BeginObject(StringRef name) = 0;
        virtual void EndObject() = 0;
        virtual void BeginArray(StringRef name) = 0;
        virtual void EndArray() = 0;
        
        virtual void Write(StringRef name, const Object& value)
        {
            Write(name, &value, value.GetObjectType());
        }

        template<typename T>
        inline void Write(StringRef name, const T& value)
        {
            Write(name, &value, MetaTable::TypeOf<T>());
        }

        virtual void Write(StringRef name, const void* value, Type type) = 0;

        virtual ~OutputArchive() = default;
    };

}// namespace Wl