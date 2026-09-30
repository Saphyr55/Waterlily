#pragma once

#include "Function.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Traits/AlignedStorage.hpp"
#include "Waterlily/Core/Traits/Traits.hpp"
#include "Waterlily/Core/Traits/TypeList.hpp"


#include <type_traits>
#include <utility>

namespace Wl
{

    class MethodInvoker
    {
    public:
        template<typename MethodType, typename ObjectType, typename ReturnType, typename... ParamTypes>
        inline static void Invoke(MethodType method, void* self, void** args, void* returnObject, TypeList<ParamTypes...>)
        {
            ObjectType* obj = static_cast<ObjectType*>(self);

            [&]<size_t... I>(std::index_sequence<I...>)
            {
                auto InvokeImpl = [&]() -> ReturnType
                {
                    return (obj->*method)(static_cast<ParamTypes>(*static_cast<std::remove_reference_t<ParamTypes>*>(args[I]))...);
                };

                if constexpr (std::is_void_v<ReturnType>)
                {
                    InvokeImpl();
                }
                else if constexpr (std::is_reference_v<ReturnType>)
                {
                    using Address = std::remove_reference_t<ReturnType>;

                    ReturnType ref = InvokeImpl();
                    Address* p = std::addressof(ref);

                    if (returnObject)
                    {
                        *static_cast<Address**>(returnObject) = p;
                    }
                }
                else
                {
                    if (returnObject)
                    {
                        WL_PLACEMENT_NEW(returnObject, ReturnType(InvokeImpl()));
                    }
                    else
                    {
                        InvokeImpl();
                    }
                }
            }(std::index_sequence_for<ParamTypes...> {});
        }
    };

    class MethodHandle
    {
        using Invoker = Function<void(void* self, void** args, void* returnObject)>;

    public:
        template<typename MethodType>
        static MethodHandle Create(MethodType method)
        {
            using Traits = MethodTraits<MethodType>;
            using ObjectType = typename Traits::Object;
            using ReturnType = typename Traits::Return;
            using Params = typename Traits::Params;

            return MethodHandle([method](void* self, void** args, void* ret)
            {
                MethodInvoker::Invoke<MethodType, ObjectType, ReturnType>(method, self, args, ret, Params());
            });
        }

    public:
        inline bool IsValid() const
        {
            return static_cast<bool>(m_invoker);
        }

        inline explicit operator bool() const
        {
            return IsValid();
        }

        template<typename R = void, typename Obj, typename... Args>
        R Invoke(Obj& object, Args&&... args) const
        {
            using AddressType = const volatile void*;

            WL_CHECK(IsValid());

            void* argv[] = {const_cast<void*>(static_cast<AddressType>(std::addressof(args)))...};
            void* self = const_cast<void*>(static_cast<AddressType>(std::addressof(object)));
            return Call<R>(self, argv);
        }

        template<typename R = void, typename Obj, typename... Args>
        R Invoke(Obj* object, Args&&... args) const
        {
            WL_CHECK(object);
            return Invoke<R>(*object, std::forward<Args>(args)...);
        }

        template<typename Obj, typename... Args>
        bool TryInvoke(Obj& object, Args&&... args) const
        {
            if (!IsValid())
            {
                return false;
            }

            Invoke<void>(object, std::forward<Args>(args)...);
            return true;
        }

        void InvokeRaw(void* self, void** args, void* returnObject) const;

    public:
        MethodHandle() = default;
        MethodHandle(Invoker invoker)
            : m_invoker(std::move(invoker))
        {
        }

    private:
        template<typename R>
        R Call(void* self, void** argv) const
        {
            AlignedStorageType<R> returnStorage;
            m_invoker(self, argv, returnStorage.Storage);

            R* value = std::launder(returnStorage.GetPtr());
            SharedPtr<R> valueGuard(value, [](R* v)
            {
                if constexpr (std::is_destructible_v<R>)
                {
                    v->~R();
                }
            });
            return std::move(*value);
        }

        template<typename R>
            requires(std::is_reference_v<R>)
        R Call(void* self, void** argv) const
        {
            std::remove_reference_t<R>* ptr = nullptr;

            m_invoker(self, argv, &ptr);

            WL_CHECK(ptr);

            return static_cast<R>(*ptr);
        }

        template<typename R>
            requires(std::is_void_v<R>)
        R Call(void* self, void** argv) const
        {
            m_invoker(self, argv, nullptr);
        }

    private:
        Invoker m_invoker;
    };

}// namespace Wl