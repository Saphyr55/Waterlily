#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

#include <ostream>

namespace Wl
{

    template<typename CharType>
    class StringBase;

    WL_CORE_API size_t StringLength(const char* str);
    WL_CORE_API size_t StringLength(const wchar_t* str);

    WL_CORE_API errno_t StringCopy(char* dst, size_t sizeInBytes, const char* src);
    WL_CORE_API errno_t StringCopy(wchar_t* dst, size_t sizeInBytes, const wchar_t* src);

    WL_CORE_API errno_t StringCat(char* dst, size_t sizeInBytes, const char* src);
    WL_CORE_API errno_t StringCat(wchar_t* dst, size_t sizeInBytes, const wchar_t* src);

    WL_CORE_API int32_t StringCompare(const char* str1, const char* str2);
    WL_CORE_API int32_t StringCompare(const wchar_t* str1, const wchar_t* str2);

    WL_CORE_API size_t StringHash(const char* str, size_t length);
    WL_CORE_API size_t StringHash(const wchar_t* str, size_t length);

    consteval uint64_t CStringPack64(const char* str)
    {
        uint64_t result = 0;
        for (size_t i = 0; i < 8; i++)
        {
            uint8_t character = static_cast<uint8_t>(str[i]);
            result |= static_cast<uint64_t>(character) << (i * 8);
        }
        return result;
    }

    /**
 * @brief Converts a UTF-8 encoded string to a wide character string (UTF-16).
 * This function allocates memory for the large string and returns it, which can cause a number of performance problems.
 *
 * @param c_str The UTF-8 encoded string to convert.
 * @return A WString containing the converted wide character string.
 */
    WL_CORE_API StringBase<wchar_t> UTF8ToWString(const char* p_str);

    template<typename CharType>
    class StringBase
    {
    public:
        size_t GetSize() const
        {
            if (m_buffer.IsEmpty())
            {
                return 0;
            }

            return m_buffer.GetSize() - 1;
        }

        size_t GetCapacity() const
        {
            return m_buffer.GetCapacity();
        }

        StringBase& Append(const CharType* str)
        {
            WL_CHECK(str);

            size_t len = StringLength(str);
            if (!m_buffer.IsEmpty())
            {
                m_buffer.Pop();// Remove null terminator.
            }
            for (size_t i = 0; i < len; i++)
            {
                m_buffer.Append(str[i]);
            }
            m_buffer.Append(CharType('\0'));
            return *this;
        }

        StringBase& Append(CharType c)
        {
            if (!m_buffer.IsEmpty())
            {
                m_buffer.Pop();// Remove null terminator.
            }
            m_buffer.Append(c);
            m_buffer.Append(CharType('\0'));
            return *this;
        }

        StringBase& Append(const StringBase& string)
        {
            Append(string.GetData());
            return *this;
        }

        const CharType* GetData() const
        {
            return m_buffer.GetData();
        }

        CharType* GetData()
        {
            return m_buffer.GetData();
        }

        void Clear()
        {
            m_buffer.Clear();
            m_buffer.Append(CharType('\0'));
        }

        void Resize(size_t size)
        {
            m_buffer.Resize(size + 1);
            m_buffer[size] = CharType('\0');
        }

        void Reserve(size_t capacity)
        {
            m_buffer.Reserve(capacity + 1);
        }

        bool IsEmpty() const
        {
            return GetSize() == 0;
        }

    public:
        StringBase()
            : m_buffer()
        {
            m_buffer.Append(CharType('\0'));
        }

        explicit StringBase(size_t capacity)
            : m_buffer()
        {
            Reserve(capacity);
            m_buffer.Append(CharType('\0'));
        }

        StringBase(const CharType* c_str)
            : m_buffer()
        {
            WL_CHECK(c_str);
            Append(c_str);
        }

        StringBase(const StringBase& other)
            : m_buffer(other.m_buffer)
        {
        }

        StringBase& operator=(const StringBase& other)
        {
            if (this != std::addressof(other))
            {
                m_buffer = other.m_buffer;
            }
            return *this;
        }

        StringBase& operator=(const CharType* c_str)
        {
            WL_CHECK(c_str);
            *this = StringBase<CharType>(c_str);
            return *this;
        }

        StringBase& operator+=(const CharType* c_str)
        {
            WL_CHECK(c_str);
            return Append(c_str);
        }

        StringBase& operator+=(const StringBase& str)
        {
            return Append(str);
        }

        StringBase& operator+=(CharType c)
        {
            if (m_buffer.GetSize() > 0)
            {
                m_buffer.Pop();
            }
            m_buffer.Append(c);
            m_buffer.Append(CharType('\0'));
            return *this;
        }

        bool operator==(const StringBase& str) const
        {
            return StringCompare(m_buffer.GetData(), str.GetData()) == 0;
        }

        const CharType& operator[](size_t i) const
        {
            return m_buffer[i];
        }

        CharType& operator[](size_t i)
        {
            return m_buffer[i];
        }

        Array<CharType>::iterator begin()
        {
            return m_buffer.begin();
        }

        Array<CharType>::const_iterator cbegin()
        {
            return m_buffer.cbegin();
        }

        Array<CharType>::iterator end()
        {
            return m_buffer.end();
        }

        Array<CharType>::const_iterator cend()
        {
            return m_buffer.cend();
        }

        const CharType* data() const
        {
            return GetData();
        }

        CharType* data()
        {
            return GetData();
        }

        void clear()
        {
            Clear();
        }

    public:
        virtual ~StringBase()
        {
        }

    private:
        void EnsureCapacity(size_t requiredCapacity)
        {
            if (requiredCapacity + 1 > m_buffer.GetCapacity())
            {
                Reserve(requiredCapacity);
            }
        }

    protected:
        Array<CharType> m_buffer;
    };

    using WString = StringBase<wchar_t>;
    using String = StringBase<char>;
    using String8 = StringBase<char8_t>;
    using String16 = StringBase<char16_t>;
    using String32 = StringBase<char32_t>;

    template<typename CharType>
    inline void operator<<(OutputStream& stream, const StringBase<CharType>& str)
    {
        const CharType* data = str.GetData();
        const uint8_t* buffer = reinterpret_cast<const uint8_t*>(data);
        stream << static_cast<uint64_t>(str.GetSize());
        stream.Write(buffer, str.GetSize() * sizeof(CharType));
    }

    /**
 * @brief The text must be resized to the expected size before calling this function, otherwise it will cause
 * undefined behavior.
 */
    template<typename CharType>
    inline bool read_text(InputStream& stream, StringBase<CharType>& text)
    {
        return stream.Read(reinterpret_cast<uint8_t*>(text.GetData()), text.GetSize() * sizeof(CharType));
    }

    template<typename CharType>
    inline void operator>>(InputStream& stream, StringBase<CharType>& str)
    {
        uint64_t size = 0;
        stream >> size;
        str.Resize(size);

        uint8_t* buffer = reinterpret_cast<uint8_t*>(str.GetData());
        stream.Read(buffer, str.GetSize() * sizeof(CharType));
    }

    template<typename CharType>
    inline std::ostream& operator<<(std::ostream& os, const StringBase<CharType>& str)
    {
        os << str.GetData();
        return os;
    }

}// namespace Wl

WL_HASH_DEFINE(Wl::String, s, { return Wl::StringHash(s.GetData(), s.GetSize()); })
WL_HASH_DEFINE(Wl::WString, s, { return Wl::StringHash(s.GetData(), s.GetSize()); })
