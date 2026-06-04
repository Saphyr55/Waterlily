#include "ACP/Mesh/MeshProcessor.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"

namespace Wl
{

    bool StaticMeshGenerateNormalsProcessor::Process(SharedPtr<Asset> assetData)
    {
        SharedPtr<StaticMesh> sma = Wl::StaticPtrCast<StaticMesh>(assetData);
        if (sma->Normals.IsEmpty())
        {
            sma->GenerateNormals();
        }
        return true;
    }

    bool StaticMeshGenerateTangentsProcessor::Process(SharedPtr<Asset> assetData)
    {
        SharedPtr<StaticMesh> sma = Wl::StaticPtrCast<StaticMesh>(assetData);
        if (sma->Tangents.IsEmpty())
        {
            sma->GenerateTangents();
        }
        return true;
    }

}// namespace Wl