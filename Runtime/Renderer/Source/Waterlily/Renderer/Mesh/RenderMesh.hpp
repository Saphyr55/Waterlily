#pragma once

#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"
#include "Waterlily/Renderer/UploadScheduler.hpp"

namespace Wl
{

    struct RenderSubMesh;
    struct RenderSubMeshData;

    struct RenderSubMeshDataLayout
    {
        size_t ModelOffset;
        size_t MaterialOffset;
        size_t Stride;

        WL_RENDERER_API void UpdateData(uint8_t* dst, const RenderSubMesh& src);
    };

    struct RenderSubMeshData
    {
        Matrix4f Model = Matrix4f::Identity();
        MaterialHandle Material = MaterialRegistry::InvalidHandle;

        WL_RENDERER_API static RenderSubMeshDataLayout CreateLayout(size_t minAlignment);
    };

    struct RenderSubMesh
    {
        Matrix4f Model = Matrix4f::Identity();
        MaterialHandle Material = MaterialRegistry::InvalidHandle;

        uint32_t VertexOffset;
        uint32_t IndexOffset;
        uint32_t VertexCount;
        uint32_t IndexCount;
    };

    struct WL_RENDERER_API RenderMesh
    {
    public:
        void Instanciate(const Array<RHIBuffer*>& vertexBuffers,
                         RHIBuffer* indexBuffer,
                         const Array<RenderSubMesh>& subMeshes);

        void Instanciate(UploadScheduler& uploader,
                         SharedPtr<AssetManager> assets,
                         SharedPtr<TextureRegistry> textures,
                         SharedPtr<MaterialRegistry> materials,
                         StaticMesh* mesh);

        void Destroy();

        const Array<RenderSubMesh>& GetSubMeshes() const;
        Array<RenderSubMesh>& GetSubMeshes();
        void AddSubMesh(const RenderSubMesh& subMesh);

        const Array<RHIBuffer*>& GetVertexBuffers() const;
        RHIBuffer* GetIndexBuffer();

        Array<RHIDrawIndexedCommand> CreateDrawIndexedCommands();

        inline size_t GetSubMeshCount()
        {
            return GetSubMeshes().GetSize();
        }

    public:
        RenderMesh(SharedPtr<RHIDevice> device);
        ~RenderMesh();

    private:
        SharedPtr<RHIDevice> m_device;
        Array<RHIBuffer*> m_vertexBuffers;
        RHIBuffer* m_indexBuffer;
        Array<RenderSubMesh> m_subMeshes;
    };

}// namespace Wl