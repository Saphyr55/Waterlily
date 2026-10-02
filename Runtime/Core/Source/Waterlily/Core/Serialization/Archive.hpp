#pragma once

#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Type.hpp"

namespace Wl
{

    class Object;

    class OutputArchive
    {
    public:
        virtual void BeginObject(StringRef name) = 0;
        virtual void EndObject() = 0;
        virtual void BeginArray(StringRef name) = 0;
        virtual void EndArray() = 0;

        virtual void Write(StringRef name, const Object& value);
        
        template<typename T>
        inline void Write(StringRef name, const T& value)
        {
            Write(name, &value, MetaTable::TypeOf<T>());
        }

        virtual void Write(StringRef name, const void* value, Type type) = 0;

        virtual ~OutputArchive() = default;
    };

    class InputArchive
    {
    public:
        virtual bool BeginObject(StringRef name) = 0;
        virtual void EndObject() = 0;
        virtual size_t BeginArray(StringRef name) = 0;
        virtual void EndArray() = 0;

        virtual bool Read(StringRef name, Object& value);

        template<typename T>
        inline bool Read(StringRef name, T& value)
        {
            return Read(name, &value, MetaTable::TypeOf<T>());
        }

        virtual bool Read(StringRef name, void* value, Type type) = 0;

        virtual ~InputArchive() = default;
    };

}// namespace Wl