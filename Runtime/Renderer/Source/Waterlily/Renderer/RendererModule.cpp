#include "Waterlily/Renderer/RendererModule.hpp"

#include "Waterlily/Assets/AssetSerializer.hpp"

#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/Renderer/Material/Material.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/Texture/TextureAsset.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(RendererModule, "Waterlily.Renderer");

    void RendererModule::OnLoad()
    {
        WL_LOG_INFO("[RendererModule]", "Renderer module loaded.");
    }

    void RendererModule::OnStartup()
    {
        WL_LOG_INFO("[RendererModule]", "Renderer module started.");

        AssetSerializer::RegisterAssetType<MaterialAsset>(AssetType_Material);
        AssetSerializer::RegisterAssetType<TextureAsset>(AssetType_Texture2D);
        AssetSerializer::RegisterAssetType<StaticMesh>(AssetType_StaticMesh);
        AssetSerializer::RegisterAssetType<Model>(AssetType_Model);
    }

    void RendererModule::OnShutdown()
    {
        AssetSerializer::UnregisterAssetType<MaterialAsset>(AssetType_Material);
        AssetSerializer::UnregisterAssetType<TextureAsset>(AssetType_Texture2D);
        AssetSerializer::UnregisterAssetType<StaticMesh>(AssetType_StaticMesh);
        AssetSerializer::UnregisterAssetType<Model>(AssetType_Model);

        WL_LOG_INFO("[RendererModule]", "Renderer module stopped.");
    }

    void RendererModule::OnUnload()
    {
        WL_LOG_INFO("[RendererModule]", "Renderer module unloaded.");
    }

}// namespace Wl