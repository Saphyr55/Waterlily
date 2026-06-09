#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Object/Field.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/Material/Material.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"
#include "Waterlily/Renderer/UploadScheduler.hpp"
#include <cstdint>


namespace Wl
{

    RenderMesh::RenderMesh(SharedPtr<RHIDevice> device)
        : m_device(device)
    {
    }

    RenderMesh::~RenderMesh()
    {
    }

    void RenderSubMeshDataLayout::UpdateData(uint8_t* dst, const RenderSubMesh& src)
    {
        Memory::Copy(dst + ModelOffset, &src.Model, sizeof(decltype(src.Model)));
        Memory::Copy(dst + MaterialOffset, &src.Material, sizeof(decltype(src.Material)));
    }

    RenderSubMeshDataLayout RenderSubMeshData::CreateLayout(size_t alignment)
    {
        size_t offset = 0;
        RenderSubMeshDataLayout layout;

        offset = FieldOffsetAlignUp<Matrix4f>(offset, alignment, layout.ModelOffset);
        offset = FieldOffsetAlignUp<MaterialHandle>(offset, alignment, layout.MaterialOffset);

        layout.Stride = Memory::AlignUp(offset, alignment);

        return layout;
    }

    const Array<RHIBuffer*>& RenderMesh::GetVertexBuffers() const
    {
        return m_vertexBuffers;
    }

    RHIBuffer* RenderMesh::GetIndexBuffer()
    {
        return m_indexBuffer;
    }

    const Array<RenderSubMesh>& RenderMesh::GetSubMeshes() const
    {
        return m_subMeshes;
    }

    Array<RenderSubMesh>& RenderMesh::GetSubMeshes()
    {
        return m_subMeshes;
    }

    void RenderMesh::AddSubMesh(const RenderSubMesh& subMesh)
    {
        m_subMeshes.Append(subMesh);
    }

    void RenderMesh::Instanciate(UploadScheduler& uploader,
                                 SharedPtr<AssetManager> assets,
                                 SharedPtr<TextureRegistry> textures,
                                 SharedPtr<MaterialRegistry> materials,
                                 StaticMesh* mesh)
    {
        m_subMeshes.Reserve(mesh->SubMeshes.GetSize());

        for (StaticMesh::SubMesh& subMesh: mesh->SubMeshes)
        {
            RenderSubMesh item = {};

            item.IndexOffset = subMesh.IndexOffset;
            item.IndexCount = subMesh.IndexCount;

            item.VertexOffset = subMesh.VertexOffset;
            item.VertexCount = subMesh.VertexCount;

            MaterialAsset* materialAsset = assets->GetAsset<MaterialAsset>(subMesh.Material);

            MaterialData materialData = {};
            materialData.DiffuseFactor = materialAsset->DiffuseFactor;
            materialData.Diffuse = textures->ObtainTexture(materialAsset->Diffuse, false);
            materialData.Normal = textures->ObtainTexture(materialAsset->Normal, true);

            item.Material = materials->ObtainMaterial(materialData);

            m_subMeshes.Append(item);
        }

        auto createVertexBuffer = [&](ArrayView<uint8_t> data) -> RHIBuffer*
        {
            RHIBufferDescription description = {};
            description.Size = data.GetSize();
            description.Usage = RHIBufferUsageFlags::Vertex | RHIBufferUsageFlags::TransferDst;
            description.SharingMode = RHISharingMode::Private;
            description.MemoryUsage = RHIMemoryUsage::Device;
            return m_device->CreateBuffer(description);
        };

        m_vertexBuffers.Resize(4);

        m_vertexBuffers[0] = createVertexBuffer(mesh->Positions);
        m_vertexBuffers[1] = createVertexBuffer(mesh->Normals);
        m_vertexBuffers[2] = createVertexBuffer(mesh->UVTextures);
        m_vertexBuffers[3] = createVertexBuffer(mesh->Tangents);

        RenderAllocation positionsAllocation =
                uploader.Upload(mesh->Positions.GetData(), mesh->Positions.GetSize(), m_vertexBuffers[0], 0);

        RenderAllocation normalsAllocation = uploader.Upload(mesh->Normals.GetData(), mesh->Normals.GetSize(), m_vertexBuffers[1], 0);

        RenderAllocation usvAllocation =
                uploader.Upload(mesh->UVTextures.GetData(), mesh->UVTextures.GetSize(), m_vertexBuffers[2], 0);

        RenderAllocation tangentsAllocation =
                uploader.Upload(mesh->Tangents.GetData(), mesh->Tangents.GetSize(), m_vertexBuffers[3], 0);

        RHIBufferDescription indexBufferDescription = {};
        indexBufferDescription.Size = mesh->Indices.GetSize() * sizeof(uint32_t);
        indexBufferDescription.Usage = RHIBufferUsageFlags::Index | RHIBufferUsageFlags::TransferDst;
        indexBufferDescription.SharingMode = RHISharingMode::Private;
        indexBufferDescription.MemoryUsage = RHIMemoryUsage::Device;

        m_indexBuffer = m_device->CreateBuffer(indexBufferDescription);
        uploader.Upload(ArrayView(mesh->Indices), m_indexBuffer, 0);
    }

    void RenderMesh::Instanciate(const Array<RHIBuffer*>& vertexBuffers,
                                 RHIBuffer* indexBuffer,
                                 const Array<RenderSubMesh>& subMeshes)
    {
        m_vertexBuffers = vertexBuffers;
        m_indexBuffer = indexBuffer;
        m_subMeshes = subMeshes;
    }

    void RenderMesh::Destroy()
    {
        m_device->DestroyBuffer(m_indexBuffer);
        for (RHIBuffer* vertex_buffer: m_vertexBuffers)
        {
            m_device->DestroyBuffer(vertex_buffer);
        }
    }

    Array<RHIDrawIndexedCommand> RenderMesh::CreateDrawIndexedCommands()
    {
        Array<RHIDrawIndexedCommand> commands(m_subMeshes.GetSize());

        for (uint32_t itemIndex = 0; itemIndex < m_subMeshes.GetSize(); itemIndex++)
        {
            RenderSubMesh& submesh = m_subMeshes[itemIndex];

            commands.Append(RHIDrawIndexedCommand {
                    .IndexCount = submesh.IndexCount,
                    .InstanceCount = 1,
                    .FirstIndex = submesh.IndexOffset,
                    .VertexOffset = submesh.VertexOffset,
                    .FirstInstance = itemIndex,
            });
        }

        return commands;
    }

}// namespace Wl