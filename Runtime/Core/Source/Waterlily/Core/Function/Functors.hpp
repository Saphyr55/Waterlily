#pragma once

#include "Waterlily/Core/Function/Function.hpp"

namespace Wl
{

    template<typename T>
    inline auto Map(T* begin, T* end, auto&& functor) -> void
    {
        for (auto it = begin; it != end; it++)
        {
            functor(*it);
        }
    }

    template<typename T>
    inline auto Reduce(T* begin, T* end, auto initiale_value, auto&& functor) -> auto
    {
        auto value = initiale_value;
        for (auto it = begin; it != end; it++)
        {
            value = functor(value, *it);
        }
        return value;
    }

    template<typename BeginIterator, typename EndIterator, typename OutputIterator, typename Func>
    inline auto Transform(BeginIterator begin, EndIterator end, OutputIterator output, Func&& func)
    {
        while (begin != end)
        {
            *output = func(*begin);

            begin++;
            output++;
        }
    }

    template<typename BeginIterator, typename EndIterator, typename T>
    inline auto Constains(BeginIterator begin, EndIterator end, const T& value) -> bool
    {
        while (begin != end)
        {
            if (*begin == value)
            {
                return true;
            }
            begin++;
        }
        return false;
    }

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