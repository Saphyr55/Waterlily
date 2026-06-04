#include "ACP/AssetImporter.hpp"
#include "ACP/AssetProcessor.hpp"
#include "ACP/AssetStorage.hpp"
#include "ACP/GlTF/GlTFImporter.hpp"
#include "ACP/Mesh/MeshProcessor.hpp"
#include "ACP/Texture/TextureAssetImporter.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/AssetSerializer.hpp"
#include "Waterlily/Assets/AssetSource.hpp"
#include "Waterlily/Assets/LCAFile.hpp"
#include "Waterlily/Assets/VFSAssetSource.hpp"
#include "Waterlily/Core/IO/FileHandle.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/Texture/TextureAsset.hpp"

#include <filesystem>

static constexpr StringRef WorkingDirectory = "";
static const StringRef VFSAssetsDirectory = "Assets/";
static const String VFSOutputAssetDirectory = VFSAssetsDirectory + "Models/Sponza/";
static constexpr StringRef OriginalAssetFilepath = "../../../Assets/Models/Sponza/glTF/Sponza.gltf";

static bool PersistAsset(FileSystem& fileSystem, StringRef output, SharedPtr<Asset>& asset)
{
    FileResult result = fileSystem.OpenWrite(output, FileMode::Create);

    if (result.HasValue())
    {
        FileHandle& file = *result.GetValue();

        LCAHeader header;
        header.AssetType = asset->AssetType;
        file << header;

        AssetSerializer::Serialize(file, *asset);
        return true;
    }

    return false;
}

int32_t main(int32_t argc, const char* argv[])
{
    LLOG_INFO("[WACP]", "Build started");

    ModuleRegistry& modules = ModuleRegistry::GetInstance();
    Module* rendererModule = modules.LoadModule("Waterlily.Renderer");
    rendererModule->OnStartup();

    FileSystem& fileSystem = FileSystem::GetPlatform();

    std::filesystem::path outputAssetDir = WorkingDirectory.data();
    outputAssetDir /= VFSOutputAssetDirectory.GetData();

    std::string outputAssetDirText = outputAssetDir.generic_string();

    if (!fileSystem.CreateDirectory(outputAssetDirText.data()))
    {
        LLOG_ERROR("[WACP]", Wl::Format("Failed to create the directory \"%s\"", VFSOutputAssetDirectory.GetData()));
        return EXIT_FAILURE;
    }

    std::filesystem::path larFilepath = WorkingDirectory.data();
    larFilepath /= VFSAssetsDirectory.data();
    larFilepath /= LAR_FILENAME.data();

    std::string larFilepathText = larFilepath.generic_string();

    FileResult larFileResult =
            fileSystem.Open(larFilepathText.data(), FileAccess::ReadWrite, FileMode::OpenOrCreate);
    if (!larFileResult.HasValue())
    {
        LLOG_ERROR("[WACP]", Wl::Format("Failed to open \"%s\"", larFilepathText.data()));
        return EXIT_FAILURE;
    }

    FileHandle& larFile = *larFileResult.GetValue();

    SharedPtr<AssetRegistry> registry =
            larFile.GetSize() == 0 ? AssetRegistry::CreateFromFile(larFile) : AssetRegistry::LoadFromFile(larFile);

    larFile.Close();

    if (!registry)
    {
        LLOG_ERROR("[WACP]", Wl::Format("Failed to load \"%s\"", larFilepathText.data()));
        return EXIT_FAILURE;
    }

    Array<SharedPtr<AssetProcessor>> staticMeshProcessors(2);
    staticMeshProcessors.Append(MakeShared<StaticMeshGenerateNormalsProcessor>());
    staticMeshProcessors.Append(MakeShared<StaticMeshGenerateTangentsProcessor>());

    AssetImporterRegistry importers;
    importers.RegisterImporter(AssetType_Texture2D, MakeShared<TextureAssetImporter>());
    importers.RegisterImporter(AssetType_Model, MakeShared<GlTFImporter>(importers));

    const StringID& assetType = AssetType_Model;
    StringRef assetFilepath = OriginalAssetFilepath;
    SharedPtr<AssetImporter> importer = importers.GetImporter(assetType);
    SharedPtr<AssetSource> source = MakeShared<VFSAssetSource>(FileSystem::GetPlatform());

    LLOG_INFO("[WACP]",
              Wl::Format("Importing, URI: \"%s\", Type: \"%s\"", assetFilepath.data(), AssetType_Model.GetText().data()));

    AssetStorage storage;

    ImportContext mainImportContext(source, registry, storage, assetType.GetText(), assetFilepath, VFSOutputAssetDirectory);
    if (SharedPtr<Asset> mainAsset = importer->ImportAsset(mainImportContext))
    {
        LLOG_INFO("[WACP]", "Importing succeeded");

        for (auto [handle, asset]: storage)
        {
            if (asset->AssetType == AssetType_StaticMesh)
            {
                for (SharedPtr<AssetProcessor>& processor: staticMeshProcessors)
                {
                    processor->Process(asset);
                }
            }
        }

        for (auto [handle, asset]: storage)
        {
            AssetMetadata& metadata = registry->GetMetadata(handle);
            std::filesystem::path assetOutputPath = WorkingDirectory.data();
            assetOutputPath /= metadata.GetURI().GetText().data();
            std::string assetOutputPathText = assetOutputPath.generic_string();
            PersistAsset(fileSystem, assetOutputPathText.data(), asset);
        }

        larFileResult = fileSystem.Open(larFilepathText.data(), FileAccess::ReadWrite, FileMode::CreateNew);

        AssetRegistry::PersistFile(registry, *larFileResult.GetValue());

        larFileResult.GetValue()->Close();
    }
    else
    {
        LLOG_ERROR("[WACP]", "Importing failed");
    }

    LLOG_INFO("[WACP]", "Build finish");

    rendererModule->OnShutdown();

    return EXIT_SUCCESS;
}
