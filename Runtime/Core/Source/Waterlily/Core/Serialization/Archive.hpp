#pragma once

#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    class Object;

    class OutputArchive
    {
    public:
        virtual void BeginObject(const StringID& name) = 0;
        virtual void EndObject() = 0;
        virtual void BeginArray(const StringID& name) = 0;
        virtual void EndArray() = 0;

        virtual void Write(const StringID& name, const Object& value);
        
        template<typename T>
        inline void Write(const StringID& name, const T& value)
        {
            Write(name, &value, MetaTable::TypeOf<T>());
        }

        virtual void Write(const StringID& name, const void* value, Type type) = 0;

        virtual ~OutputArchive() = default;
    };

    class InputArchive
    {
    public:
        virtual bool BeginObject(const StringID& name) = 0;
        virtual void EndObject() = 0;
        virtual size_t BeginArray(const StringID& name) = 0;
        virtual void EndArray() = 0;

        virtual bool Read(const StringID& name, Object& value);

        template<typename T>
        inline bool Read(const StringID& name, T& value)
        {
            return Read(name, &value, MetaTable::TypeOf<T>());
        }

        virtual bool Read(const StringID& name, void* value, Type type) = 0;

        virtual ~InputArchive() = default;
    };

}// namespace Wl