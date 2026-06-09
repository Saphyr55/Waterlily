#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class WL_CORE_API ModuleRegistry
    {
    public:
        using ModuleInitializerFunc = Function<Module*()>;

        struct LoadedModule
        {
            StringRef Name = "";
            Module* ModulePtr = nullptr;
            SharedPtr<DynamicLibrary> Library;

            LoadedModule() = default;
            LoadedModule(const StringRef name, Module* module_ref, const SharedPtr<DynamicLibrary> library)
                : Name(name)
                , ModulePtr(module_ref)
                , Library(library)
            {
            }
        };

    public:
        static ModuleRegistry& GetInstance();

    public:
        Module* LoadModule(StringRef name);
        template<typename ModuleType>
        ModuleType* LoadModule(StringRef name)
        {
            return static_cast<ModuleType*>(LoadModule(name));
        }

        void UnloadModule(StringRef name);

        bool ExistsModule(StringRef name) const;

        bool IsModuleLoaded(StringRef name) const;

        Module* GetModuleInterface(StringRef name);

        template<typename ModuleType = Module>
        ModuleType* GetModule(const StringRef name)
        {
            return static_cast<ModuleType*>(GetModuleInterface(name));
        }

        LoadedModule* GetLoadedModule(StringRef name);

        void RegisterModule(StringRef name, const ModuleInitializerFunc& initializer);
        void UnregisterModule(StringRef name);

    private:
        HashMap<StringRef, LoadedModule> m_loadedModules;
        HashMap<StringRef, ModuleInitializerFunc> m_pendingModules;
    };

    template<typename ModuleType>
    class AutoModuleRegistrant
    {
    public:
        AutoModuleRegistrant(StringRef uniqueName)
        {
            ModuleRegistry::GetInstance().RegisterModule(uniqueName, []() -> Module*
            {
                return Wl::New(DefaultAllocator::GetInstance(), ModuleType());
            });
        }
    };

}// namespace Wl

#define WL_REGISTER_MODULE_WITH_CUSTOM_VARIABLE(ModuleType, ModuleName, VariableName) \
    static const auto(VariableName) = []() {                                          \
        return ::Wl::AutoModuleRegistrant<ModuleType>(ModuleName);                    \
    }()

#define WL_REGISTER_MODULE(ModuleType, ModuleName)                                                               \
    WL_REGISTER_MODULE_WITH_CUSTOM_VARIABLE(ModuleType, ModuleName, WL_CONCAT(s_moduleRegistrant_, __COUNTER__))
