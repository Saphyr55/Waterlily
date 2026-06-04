#pragma once

#include "AssetsExports.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/MemoryPool.hpp"

namespace Wl
{

    class WL_ASSETS_API IAssetPool
    {
    public:
        virtual ~IAssetPool() = default;
    };

    template<typename AssetType>
    class AssetPool : public IAssetPool
    {
    public:
        AssetType* Allocate(AssetUUID UUID)
        {
            AssetType* memory = m_pool.Allocate();
            m_assets.Put(UUID, memory);
            return memory;
        }

        void Deallocate(AssetUUID UUID)
        {
            AssetType* asset = GetAsset(UUID);
            m_pool.Deallocate(asset);
        }

        AssetType* GetAsset(AssetUUID UUID)
        {
            return Contains(UUID) ? m_assets[UUID] : nullptr;
        }

        bool Contains(AssetUUID UUID)
        {
            return m_assets.Contains(UUID);
        }

        MemoryPool<AssetType>& GetMemoryPool()
        {
            return m_pool;
        }

    private:
        HashMap<AssetUUID, AssetType*> m_assets;
        MemoryPool<AssetType> m_pool;
    };

}// namespace Wl