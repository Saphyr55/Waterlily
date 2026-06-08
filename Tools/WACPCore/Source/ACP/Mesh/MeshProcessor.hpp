#pragma once

#include "ACP/ACPExports.hpp"
#include "ACP/AssetProcessor.hpp"

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
