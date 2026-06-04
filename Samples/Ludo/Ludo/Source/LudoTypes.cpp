#include "LudoTypes.hpp"

namespace Wl
{

    RenderMesh UIDrawElement::Instanciate(SharedPtr<RHIDevice> device, UploadScheduler& uploader)
    {
        RenderMesh mesh(device);

        RHIBuffer* uiPositionsBuffer = device->CreateBuffer(RHIBufferDescription{
                .Size = Positions.GetSizeInBytes(),
                .Usage = RHIBufferUsageFlags::Vertex | RHIBufferUsageFlags::TransferDst,
                .MemoryUsage = RHIMemoryUsage::Device,
                .SharingMode = RHISharingMode::Shared,
        });

        RHIBuffer* uiColorsBuffer = device->CreateBuffer(RHIBufferDescription{
                .Size = Colors.GetSizeInBytes(),
                .Usage = RHIBufferUsageFlags::Vertex | RHIBufferUsageFlags::TransferDst,
                .MemoryUsage = RHIMemoryUsage::Device,
                .SharingMode = RHISharingMode::Shared,
        });

        RHIBuffer* uiIndexBuffer = device->CreateBuffer(RHIBufferDescription{
                .Size = Indices.GetSizeInBytes(),
                .Usage = RHIBufferUsageFlags::Index | RHIBufferUsageFlags::TransferDst,
                .MemoryUsage = RHIMemoryUsage::Device,
                .SharingMode = RHISharingMode::Shared,
        });

        uploader.Upload(ArrayView(Positions), uiPositionsBuffer);
        uploader.Upload(ArrayView(Colors), uiColorsBuffer);
        uploader.Upload(ArrayView(Indices), uiIndexBuffer);

        RenderSubMesh subMesh = {};
        subMesh.Model = Matrix4f::Identity();
        subMesh.IndexCount = Indices.GetSize();
        subMesh.IndexOffset = 0;
        subMesh.VertexCount = Positions.GetSize();
        subMesh.VertexOffset = 0;

        mesh.Instanciate({uiPositionsBuffer, uiColorsBuffer}, uiIndexBuffer, {subMesh});

        return mesh;
    }

}// namespace Wl
