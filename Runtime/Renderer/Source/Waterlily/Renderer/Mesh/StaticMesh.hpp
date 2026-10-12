#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    inline const StringID AssetType_StaticMesh = "StaticMesh";

    struct WL_RENDERER_API StaticMesh : Asset
    {
    public:
        using Buffer = Array<uint8>;

        struct SubMesh
        {
            AssetHandle Material;
            uint32 VertexOffset = 0;
            uint32 VertexCount = 0;
            uint32 IndexOffset = 0;
            uint32 IndexCount = 0;
        };

    public:
        void GenerateNormals();
        void GenerateTangents();

        StaticMesh() noexcept
            : Asset(AssetType_StaticMesh)
        {
        }

        StaticMesh(const StaticMesh&) = delete;
        StaticMesh& operator=(const StaticMesh&) = delete;

    public:
        Buffer Positions;
        Buffer Normals;
        Buffer UVTextures;
        Buffer Tangents;
        Array<uint32> Indices;
        Array<SubMesh> SubMeshes;
    };

    WL_RENDERER_API void operator<<(OutputStream& stream, const StaticMesh& mesh);
    WL_RENDERER_API void operator>>(InputStream& stream, StaticMesh& mesh);

}// namespace Wl