#pragma once

#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

#include <cstddef>

namespace Wl
{

    struct RenderAllocation
    {
        RHIBuffer* Buffer;
        void* Data = nullptr;
        size_t Offset = 0;
        size_t Size = 0;

        template<typename MappedType>
        MappedType* Get()
        {
            return static_cast<MappedType*>(Data);
        }
    };

    class WL_RENDERER_API RenderAllocator
    {
    public:
        inline void Initialize(RHIBuffer* buffer, uint32_t minAlignment);

        inline void Reset();

        inline uintptr_t GetHead() const;

        inline RHIBuffer* GetBuffer();

        RenderAllocation Allocate(size_t size);

        template<typename DataType>
        inline RenderAllocation Allocate();

        template<typename DataType>
        inline RenderAllocation AllocateArray(size_t count);

        RenderAllocation Write(const void* data, size_t size);

        template<typename DataType>
        inline RenderAllocation Write(DataType& data);

        void Update(RenderAllocation& allocation, const void* data);

        template<typename DataType>
        inline void UpdateData(RenderAllocation& allocation, DataType& data);

        inline uint32_t GetMinAligment() const;

    public:
        RenderAllocator() = default;
        ~RenderAllocator() = default;

    private:
        uint32_t m_minAlignment = 0;
        RHIBuffer* m_buffer = nullptr;
        void* m_mapped = nullptr;
        uintptr_t m_head = 0;
    };

    inline uint32_t RenderAllocator::GetMinAligment() const
    {
        return m_minAlignment;
    }

    inline uintptr_t RenderAllocator::GetHead() const
    {
        return m_head;
    }

    inline RHIBuffer* RenderAllocator::GetBuffer()
    {
        return m_buffer;
    }

    inline void RenderAllocator::Initialize(RHIBuffer* buffer, uint32_t minAlignment)
    {
        m_minAlignment = minAlignment;
        m_buffer = buffer;
        m_head = 0;
        m_mapped = m_buffer->Map(m_head, buffer->GetSize());
    }

    inline void RenderAllocator::Reset()
    {
        m_head = 0;
    }

    template<typename DataType>
    inline RenderAllocation RenderAllocator::Allocate()
    {
        size_t stride = Memory::AlignUp(sizeof(DataType), m_minAlignment);
        return Allocate(stride);
    }

    template<typename DataType>
    inline RenderAllocation RenderAllocator::AllocateArray(size_t count)
    {
        size_t stride = Memory::AlignUp(sizeof(DataType), m_minAlignment);
        return Allocate(stride * count);
    }

    template<typename DataType>
    inline RenderAllocation RenderAllocator::Write(DataType& data)
    {
        return Write(static_cast<void*>(&data), sizeof(DataType));
    }

    template<typename DataType>
    inline void RenderAllocator::UpdateData(RenderAllocation& allocation, DataType& data)
    {
        Update(allocation, static_cast<void*>(&data));
    }

}// namespace Wl
