#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Traits/AlignedStorage.hpp"

namespace Wl
{

    template<typename ResourceType>
    class MemoryPool
    {
    public:
        union Block
        {
            AlignedStorageType<ResourceType> Storage;
            Block* Next;
        };

    public:
        void InitPool(Allocator* allocator, size_t blockCount = 64);

        ResourceType* Allocate();

        void Deallocate(ResourceType* resource);

        template<typename... Args>
        ResourceType* NewResource(Args&&... args);

        void DeleteResource(ResourceType* resource);

        void Dispose();

        template<typename Functor>
        void ForEach(Functor&& functor);

        Allocator* GetAllocator()
        {
            return m_allocator;
        }

        size_t GetBlockCount()
        {
            return m_blockCount;
        }

        size_t GetBlocksSize()
        {
            return m_blocks.GetSize();
        }

        size_t GetBlocksCapacity()
        {
            return m_blocks.GetCapacity();
        }

    public:
        MemoryPool();
        ~MemoryPool();

    private:
        void Grow();

    private:
        Allocator* m_allocator;
        Block* m_freeBlock;
        Array<Block*> m_blocks;
        size_t m_blockCount;
        size_t m_growFactor;
    };

    template<typename ResourceType>
    MemoryPool<ResourceType>::MemoryPool()
        : m_allocator(&DefaultAllocator::GetInstance())
        , m_freeBlock(nullptr)
        , m_blockCount(64)
        , m_blocks()
        , m_growFactor(2)
    {
    }

    template<typename ResourceType>
    MemoryPool<ResourceType>::~MemoryPool()
    {
        Dispose();
    }

    template<typename ResourceType>
    void MemoryPool<ResourceType>::InitPool(Allocator* allocator, size_t block_count)
    {
        m_allocator = allocator;
        m_blockCount = block_count;
        m_blocks.Resize(m_blockCount);
        Grow();
    }

    template<typename ResourceType>
    ResourceType* MemoryPool<ResourceType>::Allocate()
    {
        if (!m_freeBlock)
        {
            Grow();
        }

        Block* block = m_freeBlock;
        m_freeBlock = block->Next;
        return reinterpret_cast<ResourceType*>(&block->Storage);
    }

    template<typename ResourceType>
    void MemoryPool<ResourceType>::Deallocate(ResourceType* resource)
    {
        if (!resource)
        {
            return;
        }

        Block* block = reinterpret_cast<Block*>(resource);
        block->Next = m_freeBlock;
        m_freeBlock = block;
    }

    template<typename ResourceType>
    template<typename... Args>
    ResourceType* MemoryPool<ResourceType>::NewResource(Args&&... args)
    {
        ResourceType* resource = Allocate();
        WL_PLACEMENT_NEW(resource)
        ResourceType(std::forward<Args>(args)...);
        return resource;
    }

    template<typename ResourceType>
    void MemoryPool<ResourceType>::DeleteResource(ResourceType* resource)
    {
        if (!resource)
        {
            return;
        }

        if constexpr (std::is_destructible_v<ResourceType>)
        {
            resource->~ResourceType();
        }

        Deallocate(resource);
    }

    template<typename ResourceType>
    void MemoryPool<ResourceType>::Dispose()
    {
        for (Block* block: m_blocks)
        {
            m_allocator->Deallocate(block, m_blockCount * sizeof(Block));
        }
        m_blocks.Clear();
        m_freeBlock = nullptr;
    }

    template<typename ResourceType>
    void MemoryPool<ResourceType>::Grow()
    {
        Block* newBlock = static_cast<Block*>(m_allocator->Allocate(m_blockCount * sizeof(Block)));
        m_blocks.Append(newBlock);

        for (size_t i = 0; i < m_blockCount - 1; i++)
        {
            newBlock[i].Next = &newBlock[i + 1];
        }

        newBlock[m_blockCount - 1].Next = m_freeBlock;
        m_freeBlock = newBlock;
    }

    template<typename ResourceType>
    template<typename Functor>
    void MemoryPool<ResourceType>::ForEach(Functor&& functor)
    {
        Array<Block*> freeBlocks(64);
        for (Block* b = m_freeBlock; b != nullptr; b = b->next)
        {
            freeBlocks.Append(b);
        }

        for (Block* chunk: m_blocks)
        {
            for (size_t i = 0; i < m_blockCount; i++)
            {
                Block* current = &chunk[i];

                bool isFree = false;
                for (Block* fb: freeBlocks)
                {
                    if (fb == current)
                    {
                        isFree = true;
                        break;
                    }
                }

                if (!isFree)
                {
                    ResourceType* resource = reinterpret_cast<ResourceType*>(&current->Storage);
                    functor(resource);
                }
            }
        }
    }

}// namespace Wl