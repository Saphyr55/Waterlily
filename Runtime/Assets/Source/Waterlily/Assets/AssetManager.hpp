#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/AssetPool.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/AssetSerializer.hpp"
#include "Waterlily/Assets/AssetsExports.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    class WL_ASSETS_API AssetManager
    {
    public:
        template<typename AssetType>
        AssetType* GetAsset(StringID uri)
        {
            AssetMetadata& metadata = m_registry->GetMetadata(uri);
            return GetAsset<AssetType>(AssetHandle(metadata.GetUUID()));
        }

        template<typename AssetType>
        AssetType* GetAsset(AssetHandle handle, bool reload = false)
        {
            if (!handle.IsValid())
            {
                return nullptr;
            }

            AssetUUID uuid = handle.UUID;

            if (!m_registry->HasMetadata(uuid))
            {
                return nullptr;
            }

            AssetMetadata& metadata = m_registry->GetMetadata(uuid);

            StringID assetType = metadata.GetAssetType();

            SharedPtr<AssetPool<AssetType>> pool = GetAssetPoolOrCreate<AssetType>(assetType);
            AssetType* asset = pool->GetAsset(uuid);
            if (asset && !reload)
            {
                return asset;
            }
            else if (asset && reload)
            {
                asset->~AssetType();
                pool->Deallocate(uuid);
            }
            
            asset = pool->Allocate(uuid);
            if (SharedPtr<Stream> stream = m_loader->OpenAndValidate(metadata))
            {
                WL_PLACEMENT_NEW(asset)
                AssetType();
                AssetSerializer::Deserialize(*stream, *asset);
                return asset;
            }

            pool->Deallocate(uuid);
            return nullptr;
        }

        template<typename AssetType>
        void Unload(StringID location)
        {
            const AssetMetadata& metadata = m_registry->GetMetadata(location);
            return Unload<AssetType>(AssetHandle(metadata.GetUUID()));
        }

        template<typename AssetType>
        void Unload(AssetHandle handle)
        {
            if (!handle.IsValid())
            {
                return;
            }

            AssetUUID UUID = handle.UUID;

            if (!m_registry->HasMetadata(UUID))
            {
                return;
            }

            const AssetMetadata& metadata = m_registry->GetMetadata(UUID);

            StringID assetType = metadata.GetAssetType();

            SharedPtr<AssetPool<AssetType>> pool = GetAssetPoolOrCreate<AssetType>(assetType);

            if (!pool->Contains(metadata.GetUUID()))
            {
                return;
            }

            AssetType* asset = pool->GetAsset(metadata.GetUUID());

            pool->Deallocate(metadata.GetUUID());
        }

        template<typename AssetType>
        SharedPtr<AssetPool<AssetType>> GetAssetPool(StringID assetType)
        {
            if (!m_pools.Contains(assetType))
            {
                return nullptr;
            }
            return Wl::StaticPtrCast<AssetPool<AssetType>>(m_pools[assetType]);
        }

        template<typename AssetType>
        SharedPtr<AssetPool<AssetType>> GetAssetPoolOrCreate(StringID assetType)
        {
            SharedPtr<IAssetPool>* base_pool = m_pools.GetPtr(assetType);
            if (!base_pool)
            {
                base_pool = &m_pools.Put(assetType, Wl::MakeShared<AssetPool<AssetType>>()).Value;
            }
            return Wl::StaticPtrCast<AssetPool<AssetType>>(*base_pool);
        }

    private:
    public:
        AssetManager(const SharedPtr<AssetRegistry>& registry, const SharedPtr<IAssetLoader>& loader)
            : m_registry(registry)
            , m_loader(loader)
        {
        }
        ~AssetManager() = default;

    private:
        SharedPtr<AssetRegistry> m_registry;
        SharedPtr<IAssetLoader> m_loader;

        HashMap<StringID, SharedPtr<IAssetPool>> m_pools;
    };

}// namespace Wl