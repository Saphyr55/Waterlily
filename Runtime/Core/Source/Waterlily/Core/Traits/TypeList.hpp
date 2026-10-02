#pragma once

#include "Waterlily/Core/Defines.hpp"

#include <type_traits>
#include <utility>

namespace Wl
{

    template<typename T>
    struct TypeTag
    {
        using Type = T;
    };

    template<typename... Ts>
    struct TypeList
    {
        static constexpr usize npos = static_cast<usize>(-1);
        static constexpr usize Size = sizeof...(Ts);

        static constexpr usize GetSize()
        {
            return Size;
        }
        static constexpr bool IsEmpty()
        {
            return Size == 0;
        }

        template<typename T>
        static constexpr bool Contains()
        {
            return (std::is_same_v<T, Ts> || ...);
        }

        template<typename T>
        static constexpr usize Count()
        {
            return (usize(std::is_same_v<T, Ts>) + ... + usize(0));
        }

        template<typename T>
        static constexpr usize IndexOf()
        {
            constexpr bool matches[] = {std::is_same_v<T, Ts>..., false};
            for (usize i = 0; i < Size; ++i)
            {
                if (matches[i])
                {
                    return i;
                }
            }
            return npos;
        }

        template<template<typename> typename Pred>
        static constexpr bool Any()
        {
            return (Pred<Ts>::value || ...);
        }

        template<template<typename> typename Pred>
        static constexpr bool All()
        {
            return (Pred<Ts>::value && ...);
        }

        template<template<typename> typename Fn>
        using Transform = TypeList<Fn<Ts>...>;

        template<typename Fn>
        static constexpr void ForEach(Fn&& f)
        {
            (f(TypeTag<Ts>()), ...);
        }

        template<typename F>
        static constexpr void ForEachIndexed(F&& f)
        {
            [&]<usize... I>(std::index_sequence<I...>)
            {
                (f(TypeTag<Ts> {}, std::integral_constant<usize, I> {}), ...);
            }(std::index_sequence_for<Ts...> {});
        }
    };

}// namespace Wl