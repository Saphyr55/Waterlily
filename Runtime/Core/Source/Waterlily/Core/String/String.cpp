#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/hash/fnv-1a.hpp"

#include <cstring>

namespace Wl
{

    size_t StringLength(const char* cstr)
    {
        WL_CHECK(cstr);
        return ::strlen(cstr);
    }

    size_t StringLength(const wchar_t* wcstr)
    {
        WL_CHECK(wcstr);
        return ::wcslen(wcstr);
    }

    errno_t StringCopy(char* dst, size_t sizeInBytes, const char* src)
    {
        return ::strcpy_s(dst, sizeInBytes, src);
    }

    errno_t StringCopy(wchar_t* dst, size_t sizeInBytes, const wchar_t* src)
    {
        return ::wcscpy_s(dst, sizeInBytes, src);
    }

    errno_t StringCat(char* dst, size_t sizeInBytes, const char* src)
    {
        return ::strcat_s(dst, sizeInBytes, src);
    }

    errno_t StringCat(wchar_t* dst, size_t sizeInBytes, const wchar_t* src)
    {
        return ::wcscat_s(dst, sizeInBytes, src);
    }

    int32_t StringCompare(const char* str1, const char* str2)
    {
        return ::strcmp(str1, str2);
    }

    int32_t StringCompare(const wchar_t* str1, const wchar_t* str2)
    {
        return ::wcscmp(str1, str2);
    }

    uint64_t StringHash(const char* str, size_t length)
    {
        return fnv1a_cstr(str, length);
    }

    uint64_t StringHash(const wchar_t* str, size_t length)
    {
        return fnv1a_cstr(str, length);
    }

    WString UTF8ToWString(const char* cstr)
    {
        size_t size = StringLength(cstr) + 1;
        WString wstring(size);
        ::mbstowcs_s(&size, wstring.GetData(), size, cstr, size - 1);
        return wstring;
    }

}// namespace Wl
