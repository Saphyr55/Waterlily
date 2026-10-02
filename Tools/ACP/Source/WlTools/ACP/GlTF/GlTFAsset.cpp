#include "WlTools/ACP/GlTF/GlTFAsset.hpp"
#include "WlTools/ACP/AssetImporter.hpp"
#include "WlTools/ACP/PathConvertion.hpp"
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

    static ArrayView<uint8> GlTFGetAccessorData(tinygltf::Model& model, int32 accessorIndex)
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
        usize sizeInBytes = accessor.count * accessor.ByteStride(buffer_view);

        return {buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset, sizeInBytes};
    }

    template<typename T>
    static Array<uint32> GlTLGetIndicesType(tinygltf::Model& model, int32 accessorIndex)
    {
        ArrayView<uint8> indexBytes = GlTFGetAccessorData(model, accessorIndex);

        const tinygltf::Accessor& accessor = model.accessors[accessorIndex];

        Array<uint32> indices(accessor.count);

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];

        const usize stride = accessor.ByteStride(buffer_view);
        for (usize i = 0; i < accessor.count; i++)
        {
            const T* index_data = reinterpret_cast<const T*>(indexBytes.GetData() + i * stride);
            indices.Append(static_cast<uint32>(*index_data));
        }

        return indices;
    }

    static Array<uint32> GlTFGetIndices(tinygltf::Model& model, const tinygltf::Primitive& primitive)
    {
        tinygltf::Accessor& accessor = model.accessors[primitive.indices];
        switch (accessor.componentType)
        {
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return GlTLGetIndicesType<int8>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                return GlTLGetIndicesType<uint8>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return GlTLGetIndicesType<int16>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return GlTLGetIndicesType<uint16>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_INT:
                return GlTLGetIndicesType<int32>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return GlTLGetIndicesType<uint32>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return GlTLGetIndicesType<uint32>(model, primitive.indices);
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                return GlTLGetIndicesType<uint32>(model, primitive.indices);
        }
        return {};
    }

    static StaticMesh::SubMesh GlTFCreatePrimitive(tinygltf::Model& model,
                                                   const tinygltf::Primitive& primitive,
                                                   StaticMesh& outMesh)
    {
        StaticMesh::SubMesh submesh;

        Array<uint32> indices = GlTFGetIndices(model, primitive);

        submesh.IndexOffset = static_cast<uint32>(outMesh.Indices.GetSize());
        submesh.IndexCount = static_cast<uint32>(indices.GetSize());

        outMesh.Indices.AppendRange(indices.begin(), indices.end());

        auto positionIt = primitive.attributes.find("POSITION");
        if (positionIt != primitive.attributes.end())
        {
            ArrayView<uint8> view = GlTFGetAccessorData(model, positionIt->second);

            uint32 positionStride = sizeof(Vector3f);
            uint32 vertexOffset = static_cast<uint32>(outMesh.Positions.GetSize() / positionStride);
            uint32 vertexCount = static_cast<uint32>(view.GetSize() / positionStride);

            submesh.VertexOffset = vertexOffset;
            submesh.VertexCount = vertexCount;

            outMesh.Positions.AppendRange(view.begin(), view.end());
        }

        auto uvIt = primitive.attributes.find("TEXCOORD_0");
        if (uvIt != primitive.attributes.end())
        {
            ArrayView<uint8> view = GlTFGetAccessorData(model, uvIt->second);
            outMesh.UVTextures.AppendRange(view.begin(), view.end());
        }

        auto normalIt = primitive.attributes.find("NORMAL");
        if (normalIt != primitive.attributes.end())
        {
            ArrayView<uint8> view = GlTFGetAccessorData(model, normalIt->second);
            outMesh.Normals.AppendRange(view.begin(), view.end());
        }

        auto tangentIt = primitive.attributes.find("TANGENT");
        if (tangentIt != primitive.attributes.end())
        {
            ArrayView<uint8> view = GlTFGetAccessorData(model, tangentIt->second);
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
        uint64 uuidValue = handle.UUID.GetValue();
        StringRef format = "%s_%llu_%u";

        AssetRegistry& registry = *context.Registry;
        AssetStorage& storage = context.Storage;

        HashMap<int32, AssetHandle> textureMap;
        HashMap<int32, AssetHandle> materialMap;

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
            AssetHandle assetTextureHandle = registry.GetAssetHandle(StringID(lcaPath));

            WL_LOG_INFO("GlTFImporter", "Creating Texture2D Asset, URI: \"%s\"", lcaPath.data());

            textureMap.Put(gltfTexture.source, assetTextureHandle);
        }

        for (usize materialIndex = 0; materialIndex < model.GlTFModel.materials.size(); materialIndex++)
        {
            SharedPtr<MaterialAsset> materialAsset = MakeShared<MaterialAsset>();

            String filename = Wl::Format(format, materialAsset->AssetType.GetText(), uuidValue, materialIndex);
            filename.Append(WLCA_EXTENSION);

            std::filesystem::path pathURI = context.OutputURI.data();
            pathURI /= filename.GetData();
            std::string pathURIText = pathURI.generic_string();

            WL_LOG_INFO("GlTFImporter", "Creating Material Asset, URI: \"%s\"", pathURIText.data());

            StringRef pathURITextRef = pathURIText.data();
            StringID uri(pathURITextRef);
            AssetHandle materialHandle = registry.CreateAsset(materialAsset->AssetType, uri);

            tinygltf::Material& gltfMaterial = model.GlTFModel.materials[materialIndex];
            tinygltf::PbrMetallicRoughness& gltfPBR = gltfMaterial.pbrMetallicRoughness;

            materialAsset->baseColorFactor = {static_cast<float>(gltfPBR.baseColorFactor[0]),
                                              static_cast<float>(gltfPBR.baseColorFactor[1]),
                                              static_cast<float>(gltfPBR.baseColorFactor[2]),
                                              static_cast<float>(gltfPBR.baseColorFactor[3])};
            
            materialAsset->metallicFactor = static_cast<float>(gltfPBR.metallicFactor);
            materialAsset->roughnessFactor = static_cast<float>(gltfPBR.roughnessFactor);

            if (gltfPBR.metallicRoughnessTexture.index != -1)
            {
                tinygltf::Texture& texture = model.GlTFModel.textures[gltfPBR.metallicRoughnessTexture.index];
                AssetHandle textureAsset = textureMap[texture.source];
                materialAsset->metallicRoughness = textureAsset;
                registry.AddDependency(materialHandle, textureAsset);
            }

            if (gltfPBR.baseColorTexture.index != -1)
            {
                tinygltf::Texture& gltfBaseColorTexture = model.GlTFModel.textures[gltfPBR.baseColorTexture.index];
                AssetHandle baseColorTexture = textureMap[gltfBaseColorTexture.source];
                materialAsset->baseColor = baseColorTexture;
                registry.AddDependency(materialHandle, baseColorTexture);
            }

            if (gltfMaterial.emissiveTexture.index != -1)
            {
                tinygltf::Texture& texture = model.GlTFModel.textures[gltfMaterial.emissiveTexture.index];
                AssetHandle textureAsset = textureMap[texture.source];
                materialAsset->emissive = textureAsset;
                registry.AddDependency(materialHandle, textureAsset);
            }

            if (gltfMaterial.occlusionTexture.index != -1)
            {
                tinygltf::Texture& texture = model.GlTFModel.textures[gltfMaterial.occlusionTexture.index];
                AssetHandle textureAsset = textureMap[texture.source];
                materialAsset->occlusion = textureAsset;
                registry.AddDependency(materialHandle, textureAsset);
            }

            if (gltfMaterial.normalTexture.index != -1)
            {
                tinygltf::Texture& gltfNormalTexture = model.GlTFModel.textures[gltfMaterial.normalTexture.index];
                AssetHandle normalHandle = textureMap[gltfNormalTexture.source];
                materialAsset->normal = normalHandle;
                registry.AddDependency(materialHandle, normalHandle);
            }

            storage.Put(materialHandle, materialAsset);
            materialMap.Put(materialIndex, materialHandle);
        }

        model.Meshes.Reserve(model.GlTFModel.meshes.size());

        for (uint32 m = 0; m < model.GlTFModel.meshes.size(); m++)
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
            StringID uri = StringID(pathURITextRef);
            AssetHandle meshHandle = registry.CreateAsset(meshAsset->AssetType, uri);

            for (uint32 p = 0; p < gltfMesh.primitives.size(); p++)
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