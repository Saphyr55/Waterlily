#pragma once

#include "Waterlily/Core/Function/Callable.hpp"
#include "Waterlily/Core/Function/Function.hpp"

#include <type_traits>

namespace Wl
{

    template<typename Signature>
    class FunctionRef;

    template<typename ReturnType, typename... ArgumentTypes>
    class FunctionRef<ReturnType(ArgumentTypes...)> : public Callable
    {
    public:
        constexpr FunctionRef() = default;

        template<typename Callable, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, FunctionRef>>>
        constexpr FunctionRef(Callable&& c)
        {
            obj_ = (void*)std::addressof(c);
            m_callback = [](void* obj, ArgumentTypes&&... args) -> ReturnType
            {
                return (*reinterpret_cast<std::add_pointer_t<Callable>>(obj))(std::forward<ArgumentTypes>(args)...);
            };
        }

        ReturnType operator()(ArgumentTypes... args) const
        {
            return m_callback(obj_, std::forward<ArgumentTypes>(args)...);
        }

        explicit operator bool() const noexcept
        {
            return m_callback != nullptr;
        }

    private:
        using Callback = ReturnType (*)(void*, ArgumentTypes&&...);

        void* obj_ = nullptr;
        Callback m_callback = nullptr;
    };

    template<typename ObjectType, typename ReturnType, typename... Args>
    Function<ReturnType(Args...)> Bind(ObjectType& object, Function<ReturnType(ObjectType&, Args...)>&& func)
    {
        return [object, func = std::move(func)](Args&&... args) -> ReturnType
        {
            return func(object, std::forward<Args>(args)...);
        };
    }

    template<typename ObjectType, typename ReturnType, typename... Args>
    Function<ReturnType(Args...)> Bind(ObjectType* self, ReturnType (ObjectType::*func)(Args...))
    {
        return [self, func](Args&&... args) -> ReturnType
        {
            return (self->*func)(std::forward<Args>(args)...);
        };
    }

    template<typename ObjectType, typename ReturnType, typename... Args>
    Function<ReturnType(Args...)> Bind(const ObjectType* self, ReturnType (ObjectType::*func)(Args...) const)
    {
        return [self, func](Args&&... args) -> ReturnType
        {
            return (self->*func)(std::forward<Args>(args)...);
        };
    }

}// namespace Wl