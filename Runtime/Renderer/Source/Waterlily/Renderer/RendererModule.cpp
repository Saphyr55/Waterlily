#include "Waterlily/Renderer/RendererModule.hpp"

#include "Waterlily/Assets/AssetSerializer.hpp"

#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Renderer/Material/Material.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"
#include "Waterlily/Renderer/Texture/TextureAsset.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(RendererModule, "Waterlily.Renderer");

    void RendererModule::OnLoad()
    {
        WL_LOG_INFO("RendererModule", "Renderer module loaded.");
    }

    void RendererModule::OnStartup()
    {
        WL_LOG_INFO("RendererModule", "Renderer module started.");

        AssetSerializer::RegisterAssetType<MaterialAsset>(AssetType_Material);
        AssetSerializer::RegisterAssetType<TextureAsset>(AssetType_Texture2D);
        AssetSerializer::RegisterAssetType<StaticMesh>(AssetType_StaticMesh);
        AssetSerializer::RegisterAssetType<Model>(AssetType_Model);
        AssetSerializer::RegisterAssetType<Shader>(AssetType_Shader);
    }

    void RendererModule::OnShutdown()
    {
        AssetSerializer::UnregisterAssetType<MaterialAsset>(AssetType_Material);
        AssetSerializer::UnregisterAssetType<TextureAsset>(AssetType_Texture2D);
        AssetSerializer::UnregisterAssetType<StaticMesh>(AssetType_StaticMesh);
        AssetSerializer::UnregisterAssetType<Model>(AssetType_Model);
        AssetSerializer::UnregisterAssetType<Shader>(AssetType_Shader);

        WL_LOG_INFO("RendererModule", "Renderer module stopped.");
    }

    void RendererModule::OnUnload()
    {
        WL_LOG_INFO("RendererModule", "Renderer module unloaded.");
    }

}// namespace Wl