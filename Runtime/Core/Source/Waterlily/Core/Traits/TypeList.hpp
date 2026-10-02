#pragma once

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
        static constexpr size_t npos = static_cast<size_t>(-1);
        static constexpr size_t Size = sizeof...(Ts);

        static constexpr size_t GetSize()
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
        static constexpr size_t Count()
        {
            return (size_t(std::is_same_v<T, Ts>) + ... + size_t(0));
        }

        template<typename T>
        static constexpr size_t IndexOf()
        {
            constexpr bool matches[] = {std::is_same_v<T, Ts>..., false};
            for (size_t i = 0; i < Size; ++i)
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
            [&]<size_t... I>(std::index_sequence<I...>)
            {
                (f(TypeTag<Ts> {}, std::integral_constant<size_t, I> {}), ...);
            }(std::index_sequence_for<Ts...> {});
        }
    };

}// namespace Wl