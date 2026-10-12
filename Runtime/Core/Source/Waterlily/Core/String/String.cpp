#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/hash/fnv-1a.hpp"

#include <cstring>

namespace Wl
{

    usize StringLength(const char* cstr)
    {
        WL_CHECK(cstr);
        return ::strlen(cstr);
    }

    usize StringLength(const wchar_t* wcstr)
    {
        WL_CHECK(wcstr);
        return ::wcslen(wcstr);
    }

    errno_t StringCopy(char* dst, usize sizeInBytes, const char* src)
    {
        return ::strcpy_s(dst, sizeInBytes, src);
    }

    errno_t StringCopy(wchar_t* dst, usize sizeInBytes, const wchar_t* src)
    {
        return ::wcscpy_s(dst, sizeInBytes, src);
    }

    errno_t StringCat(char* dst, usize sizeInBytes, const char* src)
    {
        return ::strcat_s(dst, sizeInBytes, src);
    }

    errno_t StringCat(wchar_t* dst, usize sizeInBytes, const wchar_t* src)
    {
        return ::wcscat_s(dst, sizeInBytes, src);
    }

    int32 StringCompare(const char* str1, const char* str2)
    {
        return ::strcmp(str1, str2);
    }

    int32 StringCompare(const wchar_t* str1, const wchar_t* str2)
    {
        return ::wcscmp(str1, str2);
    }

    constexpr uint64 StringHash(const char* str, usize length)
    {
        return fnv1a_cstr(str, length);
    }

    constexpr uint64 StringHash(const wchar_t* str, usize length)
    {
        return fnv1a_cstr(str, length);
    }

    WString UTF8ToWString(const char* cstr)
    {
        usize size = StringLength(cstr) + 1;
        WString wstring(size);
        ::mbstowcs_s(&size, wstring.GetData(), size, cstr, size - 1);
        return wstring;
    }

}// namespace Wl
