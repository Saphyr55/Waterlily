#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    struct DynamicLibrary
    {
        void* Handle = nullptr;
        StringRef Name = nullptr;
    };

    class DynamicLibraryLoader
    {
    public:
        WL_CORE_API static StringRef Extension();

        WL_CORE_API static SharedPtr<DynamicLibrary> Load(StringRef name);

        WL_CORE_API static void Unload(SharedPtr<DynamicLibrary> library);

        WL_CORE_API static void* LoadFunction(SharedPtr<DynamicLibrary> library, StringRef functionName);

        template<typename FuncType>
        static FuncType LoadFunction(SharedPtr<DynamicLibrary> library, StringRef functionName)
        {
            return reinterpret_cast<FuncType>(LoadFunction(library, functionName));
        }
    };

}// namespace Wl