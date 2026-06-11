#include "ACP/GlTF/GlTFAsset.hpp"
#include "ACP/AssetImporter.hpp"
#include "ACP/PathConvertion.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Renderer/Material/Material.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/Texture/TextureAsset.hpp"

namespace Wl
{

    static ArrayView<uint8_t> GlTFGetAccessorData(tinygltf::Model& model, int32_t accessorIndex)
    {
        if (accessorIndex < 0 || accessorIndex >= model.accessors.size())
        {
            return {};
        }

        const tinygltf::Accessor& accessor = model.accessors[accessorIndex];
        if (accessor.bufferView < 0 || accessor.bufferView >= model.bufferViews.size())
        {
            return {};
        }

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
        if (buffer_view.buffer < 0 || buffer_view.buffer > model.buffers.size())
        {
            return {};
        }

        tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
        size_t sizeInBytes = accessor.count * accessor.ByteStride(buffer_view);

        return {buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset, sizeInBytes};
    }

    template<typename T>
    static Array<uint32_t> GlTLGetIndicesType(tinygltf::Model& model, int32_t accessorIndex)
    {
        ArrayView<uint8_t> indexBytes = GlTFGetAccessorData(model, accessorIndex);

        const tinygltf::Accessor& accessor = model.accessors[accessorIndex];

        Array<uint32_t> indices(accessor.count);

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];

        const size_t stride = accessor.ByteStride(buffer_view);
        for (size_t i = 0; i < accessor.count; i++)
        {
            const T* index_data = reinterpret_cast<const T*>(indexBytes.GetData() + i * stride);
            indices.Append(static_cast<uint32_t>(*index_data));
        }

        return indices;
    }

    static Array<uint32_t> GlTFGetIndices(tinygltf::Model& model, const tinygltf::Primitive& primitive)
    {
        tinygltf::Accessor& accessor = model.accessors[primitive.indices];
        switch (accessor.componentType)
        {
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return GlTLGetIndicesType<int8_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                return GlTLGetIndicesType<uint8_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return GlTLGetIndicesType<int16_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return GlTLGetIndicesType<uint16_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_INT:
                return GlTLGetIndicesType<int32_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return GlTLGetIndicesType<uint32_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return GlTLGetIndicesType<uint32_t>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                return GlTLGetIndicesType<uint32_t>(model, primitive.indices);
        }
        return {};
    }

    static StaticMesh::SubMesh GlTFCreatePrimitive(tinygltf::Model& model,
                                                   const tinygltf::Primitive& primitive,
                                                   StaticMesh& outMesh)
    {
        StaticMesh::SubMesh submesh;

        Array<uint32_t> indices = GlTFGetIndices(model, primitive);

        submesh.IndexOffset = static_cast<uint32_t>(outMesh.Indices.GetSize());
        submesh.IndexCount = static_cast<uint32_t>(indices.GetSize());

        outMesh.Indices.AppendRange(indices.begin(), indices.end());

        auto positionIt = primitive.attributes.find("POSITION");
        if (positionIt != primitive.attributes.end())
        {
            ArrayView<uint8_t> view = GlTFGetAccessorData(model, positionIt->second);

            uint32_t positionStride = sizeof(Vector3f);
            uint32_t vertexOffset = static_cast<uint32_t>(outMesh.Positions.GetSize() / positionStride);
            uint32_t vertexCount = static_cast<uint32_t>(view.GetSize() / positionStride);

            submesh.VertexOffset = vertexOffset;
            submesh.VertexCount = vertexCount;

            outMesh.Positions.AppendRange(view.begin(), view.end());
        }

        auto uvIt = primitive.attributes.find("TEXCOORD_0");
        if (uvIt != primitive.attributes.end())
        {
            ArrayView<uint8_t> view = GlTFGetAccessorData(model, uvIt->second);
            outMesh.UVTextures.AppendRange(view.begin(), view.end());
        }

        auto normalIt = primitive.attributes.find("NORMAL");
        if (normalIt != primitive.attributes.end())
        {
            ArrayView<uint8_t> view = GlTFGetAccessorData(model, normalIt->second);
            outMesh.Normals.AppendRange(view.begin(), view.end());
        }

        auto tangentIt = primitive.attributes.find("TANGENT");
        if (tangentIt != primitive.attributes.end())
        {
            ArrayView<uint8_t> view = GlTFGetAccessorData(model, tangentIt->second);
            outMesh.Tangents.AppendRange(view.begin(), view.end());
        }

        return submesh;
    }

    bool GlTFConstruct(AssetHandle handle,
                       GlTFAsset& model,
                       AssetImporterRegistry& importers,
                       ImportContext& context,
                       StringRef basedir)
    {
        uint64_t uuidValue = handle.UUID.GetValue();
        StringRef format = "%s_%llu_%u";

        AssetRegistry& registry = *context.Registry;
        AssetStorage& storage = context.Storage;

        HashMap<int32_t, AssetHandle> textureMap;
        HashMap<int32_t, AssetHandle> materialMap;

        for (const tinygltf::Texture& gltfTexture: model.GlTFModel.textures)
        {
            const tinygltf::Image& gltfImage = model.GlTFModel.images[gltfTexture.source];

            SharedPtr<AssetImporter> textureImporter = importers.GetImporter(AssetType_Texture2D);

            std::filesystem::path imagePathURI = basedir.data();
            imagePathURI /= gltfImage.uri;
            std::string imagePathURIText = imagePathURI.generic_string();

            ImportContext textureImportContext = context;
            textureImportContext.URI = imagePathURIText.data();

            SharedPtr<Asset> textureAsset = textureImporter->ImportAsset(textureImportContext);
            if (!textureAsset)
            {
                return false;
            }

            String lcaPath = PathToLCAFilepath(textureImportContext.URI, textureImportContext.OutputURI);
            AssetHandle assetTextureHandle = registry.GetAssetHandle(CreateSID(lcaPath));

            textureMap.Put(gltfTexture.source, assetTextureHandle);
        }

        for (size_t materialIndex = 0; materialIndex < model.GlTFModel.materials.size(); materialIndex++)
        {
            SharedPtr<MaterialAsset> materialAsset = MakeShared<MaterialAsset>();

            String filename = Wl::Format(format, materialAsset->AssetType.GetText(), uuidValue, materialIndex);
            filename.Append(WLCA_EXTENSION);

            std::filesystem::path pathURI = context.OutputURI.data();
            pathURI /= filename.GetData();
            std::string pathURIText = pathURI.generic_string();

            WL_LOG_INFO("GlTFImporter","Creating Material Asset, URI: \"%s\"", pathURIText.data());

            StringRef pathURITextRef = pathURIText.data();
            StringID uri = CreateSID(pathURITextRef);
            AssetHandle materialHandle = registry.CreateAsset(materialAsset->AssetType, uri);

            tinygltf::Material& gltfMaterial = model.GlTFModel.materials[materialIndex];
            tinygltf::PbrMetallicRoughness& gltfPBR = gltfMaterial.pbrMetallicRoughness;

            materialAsset->DiffuseFactor = {static_cast<float>(gltfPBR.baseColorFactor[0]),
                                            static_cast<float>(gltfPBR.baseColorFactor[1]),
                                            static_cast<float>(gltfPBR.baseColorFactor[2]),
                                            static_cast<float>(gltfPBR.baseColorFactor[3])};

            if (gltfMaterial.normalTexture.index != -1)
            {
                tinygltf::Texture& gltfBaseColorTexture = model.GlTFModel.textures[gltfPBR.baseColorTexture.index];
                AssetHandle baseColorTexture = textureMap[gltfBaseColorTexture.source];
                materialAsset->Diffuse = baseColorTexture;
                registry.AddDependency(materialHandle, baseColorTexture);
            }

            if (gltfMaterial.normalTexture.index != -1)
            {
                tinygltf::Texture& gltfNormalTexture = model.GlTFModel.textures[gltfMaterial.normalTexture.index];
                AssetHandle normalHandle = textureMap[gltfNormalTexture.source];
                materialAsset->Normal = normalHandle;
                registry.AddDependency(materialHandle, normalHandle);
            }

            storage.Put(materialHandle, materialAsset);
            materialMap.Put(materialIndex, materialHandle);
        }

        model.Meshes.Reserve(model.GlTFModel.meshes.size());

        for (uint32_t m = 0; m < model.GlTFModel.meshes.size(); m++)
        {
            tinygltf::Mesh& gltfMesh = model.GlTFModel.meshes[m];

            SharedPtr<StaticMesh> meshAsset = MakeShared<StaticMesh>();

            String filename = Wl::Format(format, meshAsset->AssetType.GetText(), uuidValue, m);
            filename.Append(WLCA_EXTENSION);

            std::filesystem::path pathURI = context.OutputURI.data();
            pathURI /= filename.GetData();
            std::string pathURIText = pathURI.generic_string();

            WL_LOG_INFO("GlTFImporter", "Creating StaticMesh, URI: \"%s\"", pathURIText.data());

            StringRef pathURITextRef = pathURIText.data();
            StringID uri = CreateSID(pathURITextRef);
            AssetHandle meshHandle = registry.CreateAsset(meshAsset->AssetType, uri);

            for (uint32_t p = 0; p < gltfMesh.primitives.size(); p++)
            {
                tinygltf::Primitive& gltfSubMesh = gltfMesh.primitives[p];

                StaticMesh::SubMesh subMesh = GlTFCreatePrimitive(model.GlTFModel, gltfSubMesh, *meshAsset);

                if (gltfSubMesh.material != -1)
                {
                    subMesh.Material = materialMap[gltfSubMesh.material];
                    registry.AddDependency(meshHandle, subMesh.Material);
                }

                meshAsset->SubMeshes.push_back(subMesh);
            }

            registry.AddDependency(handle, meshHandle);
            storage.Put(meshHandle, meshAsset);
            model.Meshes.push_back(meshHandle);
        }

        return true;
    }

}// namespace Wl