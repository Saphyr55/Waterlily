#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"

#ifdef _WIN32

#include "Waterlily/Core/Platform/Windows/Windows.hpp"

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    StringRef DynamicLibraryLoader::Extension()
    {
        return ".dll";
    }

    SharedPtr<DynamicLibrary> DynamicLibraryLoader::Load(StringRef name)
    {
        WString wname = UTF8ToWString(name.data());
        HMODULE handle = ::LoadLibraryW(wname.GetData());

        if (!handle)
        {
            return SharedPtr<DynamicLibrary>(nullptr);
        }

        return MakeShared<DynamicLibrary>(handle, name);
    }

    void DynamicLibraryLoader::Unload(SharedPtr<DynamicLibrary> library)
    {
        WL_CHECK(library);

        HMODULE handle = static_cast<HMODULE>(library->Handle);
        WL_CHECK(handle);

        ::FreeLibrary(handle);
    }

    void* DynamicLibraryLoader::LoadFunction(SharedPtr<DynamicLibrary> library, StringRef functionName)
    {
        WL_CHECK(library);
        WL_CHECK(library->Handle);

        HMODULE handle = static_cast<HMODULE>(library->Handle);
        FARPROC Function = ::GetProcAddress(handle, functionName);

        if (Function == nullptr)
        {
            return nullptr;
        }

        return reinterpret_cast<void*>(Function);
    }

}// namespace Wl

#endif// WIN32