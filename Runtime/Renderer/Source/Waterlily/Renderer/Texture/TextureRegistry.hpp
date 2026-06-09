#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/BindlessShaderResources.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Texture/Texture.hpp"

namespace Wl
{

    using TextureHandle = uint32_t;

    class WL_RENDERER_API TextureRegistry
    {
    public:
        static constexpr TextureHandle InvalidTexture = UINT32_MAX;
        static constexpr const uint32_t MaxResources = 1024u;

    public:
        TextureHandle ObtainTexture(AssetHandle asset, bool normalize = false);

        void Upload(bool release_image = true);

        void CompileSRG();

        void Destroy();

        inline const HashMap<TextureHandle, Texture>& GetRegistry() const
        {
            return m_registry;
        }

        inline HashMap<TextureHandle, Texture>& GetRegistry()
        {
            return m_registry;
        }

        inline Texture& GetDummyTexture()
        {
            return m_dummyTexture;
        }

        inline RHIShaderResourceGroupLayout* GetSRGLayout()
        {
            return m_srgLayout;
        }

        inline RHIShaderResourceGroup* GetSRG()
        {
            return m_group;
        }

    public:
        TextureRegistry(const SharedPtr<RHIDevice>& device, AssetManager& assets, uint32_t binding = 0)
            : m_device(device)
            , m_assetManager(assets)
            , m_registry(16)
            , m_count(0)
            , m_dummyTexture()
            , m_binding(binding)
        {
            InitDummyTexture();
            InitSRG();
        }

    private:
        void InitSRG();
        void InitDummyTexture();

    private:
        struct PendingUploadTexture
        {
            AssetHandle Asset;
            TextureHandle Handle;
        };

        SharedPtr<RHIDevice> m_device;
        AssetManager& m_assetManager;

        uint32_t m_binding;

        RHIBindlessShaderResources* m_bindlessResources;
        RHIShaderResourceGroupLayout* m_srgLayout;
        RHIShaderResourceGroup* m_group;

        Array<PendingUploadTexture> m_pendings;
        HashMap<AssetHandle, TextureHandle> m_handles;
        HashMap<TextureHandle, Texture> m_registry;
        TextureHandle m_count;
        Texture m_dummyTexture;
    };

}// namespace Wl