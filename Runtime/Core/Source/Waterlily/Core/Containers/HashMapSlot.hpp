#pragma once

#include "Waterlily/Core/Traits/AlignedStorage.hpp"

namespace Wl
{

    enum class SlotState : uint8_t
    {
        Occupied = 0,
        Empty = 1,
        Removed = 2,
    };

    template<typename KeyType, typename ValueType>
    struct BufferedSlot
    {
        using KeyStorage = AlignedStorage<KeyType>;
        using ValueStorage = AlignedStorage<ValueType>;

        KeyStorage::Type Key;
        ValueStorage::Type Value;
        SlotState State = SlotState::Empty;

        void Destroy()
        {
            if (State == SlotState::Occupied)
            {
                Key.Destroy();
                Value.Destroy();
            }
            State = SlotState::Removed;
        }

        void EmplaceValue(const ValueType& value)
        {
            Value.Emplace(value);
        }

        void EmplaceValue(ValueType&& value)
        {
            Value.Emplace(std::move(value));
        }

        void EmplaceKey(const KeyType& key)
        {
            Key.Emplace(key);
        }

        void Emplace(const KeyType& key, const ValueType& value)
        {
            Key.Emplace(key);
            Value.Emplace(value);
            State = SlotState::Occupied;
        }

        void Emplace(const KeyType& key, ValueType&& value)
        {
            Key.Emplace(key);
            Value.Emplace(value);
            State = SlotState::Occupied;
        }

        inline bool IsRemoved() const
        {
            return State == SlotState::Removed;
        }

        inline bool IsOccupied() const
        {
            return State == SlotState::Occupied;
        }

        inline bool IsEmpty() const
        {
            return State == SlotState::Empty;
        }
    };

}// namespace Wl
