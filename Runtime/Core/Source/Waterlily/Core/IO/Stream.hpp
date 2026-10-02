#pragma once

#include "Waterlily/Core/Defines.hpp"

#include <type_traits>

namespace Wl
{

    class InputStream
    {
    public:
        virtual bool Read(uint8* destination, usize nbytes) = 0;

        virtual ~InputStream() = default;
    };

    class OutputStream
    {
    public:
        virtual bool Write(const uint8* source, usize nbytes) = 0;

        virtual bool Flush() = 0;

        virtual ~OutputStream() = default;
    };

    class Stream
        : public InputStream
        , public OutputStream
    {
    public:
        virtual bool Read(uint8* destination, usize nbytes) = 0;

        virtual bool Write(const uint8* source, usize nbytes) = 0;

        virtual bool Flush() = 0;

        virtual int64 Tell() = 0;

        virtual bool Seek(int64 position) = 0;

        virtual usize GetSize() = 0;

        virtual ~Stream() = default;
    };

    template<typename ObjectType>
    concept WritableStream = requires(OutputStream& out, const ObjectType& object) { out << object; };

    template<typename ObjectType>
    concept ReadableStream = requires(InputStream& in, ObjectType& object) { in >> object; };

    template<typename Type>
        requires(std::is_arithmetic_v<Type>)
    inline void operator<<(OutputStream& stream, const Type& type)
    {
        stream.Write(reinterpret_cast<const uint8*>(&type), sizeof(Type));
    }

    template<typename Type>
        requires(std::is_arithmetic_v<Type>)
    inline void operator>>(InputStream& stream, Type& type)
    {
        stream.Read(reinterpret_cast<uint8*>(&type), sizeof(Type));
    }

}// namespace Wl
