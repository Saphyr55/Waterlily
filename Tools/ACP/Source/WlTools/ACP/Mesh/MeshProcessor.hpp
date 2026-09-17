#pragma once

#include "WlTools/ACP/ACPExports.hpp"
#include "WlTools/ACP/AssetProcessor.hpp"

namespace Wl
{

    class WL_TOOLS_ACP_API StaticMeshGenerateNormalsProcessor : public AssetProcessor
    {
    public:
        virtual bool Process(SharedPtr<Asset> assetData) override;

    public:
        StaticMeshGenerateNormalsProcessor() = default;
        ~StaticMeshGenerateNormalsProcessor() = default;
    };

    class WL_TOOLS_ACP_API StaticMeshGenerateTangentsProcessor : public AssetProcessor
    {
    public:
        virtual bool Process(SharedPtr<Asset> assetData) override;
    
    public:
        StaticMeshGenerateTangentsProcessor() = default;
        ~StaticMeshGenerateTangentsProcessor() = default;
    };


}// namespace Wl
