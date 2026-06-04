#pragma once

#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    class InputStream
    {
    public:
        virtual bool Read(uint8_t* destination, size_t nbytes) = 0;

        virtual ~InputStream() = default;
    };

    class OutputStream
    {
    public:
        virtual bool Write(const uint8_t* source, size_t nbytes) = 0;

        virtual bool Flush() = 0;

        virtual ~OutputStream() = default;
    };

    class Stream : public InputStream
        , public OutputStream
    {
    public:
        virtual bool Read(uint8_t* destination, size_t nbytes) = 0;

        virtual bool Write(const uint8_t* source, size_t nbytes) = 0;

        virtual bool Flush() = 0;

        virtual int64_t Tell() = 0;

        virtual bool Seek(int64_t position) = 0;

        virtual size_t GetSize() = 0;

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
        stream.Write(reinterpret_cast<const uint8_t*>(&type), sizeof(Type));
    }

    template<typename Type>
        requires(std::is_arithmetic_v<Type>)
    inline void operator>>(InputStream& stream, Type& type)
    {
        stream.Read(reinterpret_cast<uint8_t*>(&type), sizeof(Type));
    }

}// namespace Wl
