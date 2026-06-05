#pragma once

#include "Waterlily/Core/Memory/Memory.hpp"

#include <type_traits>

namespace Wl
{

    template<typename T>
    struct AlignedStorage
    {

        static constexpr const size_t Size = sizeof(T);
        static constexpr const size_t Alignment = alignof(T);

        struct Type
        {
            alignas(Alignment) uint8_t Storage[Size];

            void Emplace(const T& value)
            {
                WL_PLACEMENT_NEW(Storage)
                T(value);
            }

            void Emplace(T&& value)
            {
                WL_PLACEMENT_NEW(Storage)
                T(std::move(value));
            }

            void Destroy()
            {
                if constexpr (std::is_destructible_v<T>)
                {
                    GetRef().~T();
                }
            }

            T* GetPtr()
            {
                return reinterpret_cast<T*>(Storage);
            }

            T& GetRef()
            {
                return *GetPtr();
            }

            const T* GetPtr() const
            {
                return reinterpret_cast<const T*>(Storage);
            }

            const T& GetRef() const
            {
                return *GetPtr();
            }
        };
    };

    template<typename T>
    using AlignedStorageType = typename AlignedStorage<T>::Type;

}// namespace Wl