#pragma once

#include "Waterlily/Core/Memory/Memory.hpp"

#include <type_traits>

namespace Wl
{

    template<typename T>
    struct AlignedStorage
    {

        static constexpr const usize Size = sizeof(T);
        static constexpr const usize Alignment = alignof(T);

        struct Type
        {
            alignas(Alignment) uint8 Storage[Size];

            void Emplace(const T& value)
            {
                WL_PLACEMENT_NEW(Storage, T(value));
            }

            void Emplace(T&& value)
            {
                WL_PLACEMENT_NEW(Storage, T(std::move(value)));
            }

            void Destroy()
            {
                SafeDestruct<T>(GetPtr());
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