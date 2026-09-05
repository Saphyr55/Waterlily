#include "Waterlily/Renderer/Model/Model.hpp"

namespace Wl
{

    void operator<<(OutputStream& stream, const Model& asset)
    {
        stream << static_cast<uint64_t>(asset.Meshes.GetSize());
        for (const AssetHandle& handle: asset.Meshes)
        {
            stream << handle;
        }
    }

    void operator>>(InputStream& stream, Model& asset)
    {
        uint64_t size = 0;
        stream >> size;
        asset.Meshes.Resize(size);
        for (AssetHandle& handle: asset.Meshes)
        {
            stream >> handle;
        }
    }

    Array<StaticMesh*> Model::GetMeshes(Model* model, SharedPtr<AssetManager> assetManager)
    {
        Array<StaticMesh*> modelStaticMeshesAsset;
        modelStaticMeshesAsset.Reserve(model->Meshes.GetSize());

        for (const AssetHandle& meshAssetHandle: model->Meshes)
        {
            StaticMesh* staticMesh = assetManager->GetAsset<StaticMesh>(meshAssetHandle);
            modelStaticMeshesAsset.Append(staticMesh);
        }
        
        return modelStaticMeshesAsset;
    }


}// namespace Wl
