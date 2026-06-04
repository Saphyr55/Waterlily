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
        Vector4f DiffuseFactor;
        AssetHandle Diffuse;// Texture2D
        AssetHandle Normal; // Texture2D

        MaterialAsset()
            : Asset(AssetType_Material)
        {
        }
    };

    struct MaterialData
    {
        Vector4f DiffuseFactor;
        TextureHandle Diffuse = TextureRegistry::InvalidTexture;
        TextureHandle Normal = TextureRegistry::InvalidTexture;

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
        size_t hash = Wl::Hasher::hash(material.DiffuseFactor.x);
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.DiffuseFactor.y));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.DiffuseFactor.z));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.DiffuseFactor.w));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.Diffuse));
        hash = Wl::HashCombine(hash, Wl::Hasher::hash(material.Normal));
        return hash;
    }
};