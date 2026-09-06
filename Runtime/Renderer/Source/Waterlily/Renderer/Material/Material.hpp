#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Math/Vector4.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"

namespace Wl
{

    inline const StringID AssetType_Material = WL_SID("Material");

    struct MaterialAsset : Asset
    {
        Vector4f baseColorFactor;
        float metallicFactor;
        float roughnessFactor;

        AssetHandle baseColor;
        AssetHandle normal;
        AssetHandle emissive;
        AssetHandle occlusion;
        AssetHandle metallicRoughness;

        MaterialAsset()
            : Asset(AssetType_Material)
        {
        }
    };

    struct MaterialData
    {
        Vector4f baseColorFactor;
        float metallicFactor;
        float roughnessFactor;

        TextureHandle baseColor = TextureRegistry::InvalidTexture;
        TextureHandle normal = TextureRegistry::InvalidTexture;
        TextureHandle emissive = TextureRegistry::InvalidTexture;
        TextureHandle occlusion = TextureRegistry::InvalidTexture;
        TextureHandle metallicRoughness = TextureRegistry::InvalidTexture;

        MaterialData() = default;
        ~MaterialData() = default;

        inline bool operator==(const MaterialData& other) const = default;
        inline bool operator!=(const MaterialData& other) const = default;
    };

    WL_RENDERER_API void operator<<(OutputStream& stream, const MaterialAsset& asset);
    WL_RENDERER_API void operator>>(InputStream& stream, MaterialAsset& asset);

}// namespace Wl

template<>
struct std::hash<Wl::MaterialData>
{
    size_t operator()(const Wl::MaterialData& material) const noexcept
    {
        size_t hash = 0;

        hash = Wl::Hasher::hash(material.baseColorFactor.x);
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.baseColorFactor.y));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.baseColorFactor.z));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.baseColorFactor.w));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.metallicFactor));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.roughnessFactor));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.baseColor));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.normal));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.emissive));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.occlusion));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.metallicRoughness));
        
        return hash;
    }
};