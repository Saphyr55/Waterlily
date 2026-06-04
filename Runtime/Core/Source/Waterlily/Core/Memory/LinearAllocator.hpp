#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/TypedAllocator.hpp"

namespace Wl
{

    class WL_CORE_API LinearAllocator : public AlignedAllocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment = 1) override;

        virtual void Deallocate(void* block, size_t size, size_t alignment = 1) override;

    public:
        void Init(size_t size);

        void Destroy();

        void Reset();

        inline size_t GetSize() const
        {
            return m_size;
        }

        inline size_t GetOffset() const
        {
            return m_offset;
        }

        inline bool IsValid() const
        {
            return m_buffer != nullptr;
        }

    public:
        explicit LinearAllocator(size_t size);
        LinearAllocator();
        ~LinearAllocator();

    private:
        uint8_t* m_buffer;
        size_t m_size;
        size_t m_offset;
    };

    template<typename Type>
    class TypedLinearAllocator : public TypedAllocator<Type, LinearAllocator>
    {
    public:
        using Super = TypedAllocator<Type, LinearAllocator>;

    public:
        void Reset()
        {
            Super::GetAllocator()->Reset();
        }

    public:
        TypedLinearAllocator(LinearAllocator* allocator)
            : Super(allocator)
        {
        }
    };

}// namespace Wl