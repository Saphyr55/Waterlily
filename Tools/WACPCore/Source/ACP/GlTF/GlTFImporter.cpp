#include "ACP/GlTF/GlTFImporter.hpp"
#include "ACP/GlTF/GlTFAsset.hpp"
#include "ACP/PathConvertion.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"

#include <filesystem>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

namespace Wl
{

    static bool DummyLoaderImage(tinygltf::Image* /* image */,
                                 const int /* image_idx */,
                                 std::string* /* err */,
                                 std::string* /* warn */,
                                 int /* req_width */,
                                 int /* req_height */,
                                 const unsigned char* /* bytes */,
                                 int /* size */,
                                 void* /*user_data */)
    {
        return true;
    }

    SharedPtr<Asset> GlTFImporter::ImportAsset(ImportContext& ctx)
    {
        SharedPtr<Stream> stream = ctx.Source->OpenAsset(ctx.URI);

        std::filesystem::path modelPathURI = ctx.URI.data();
        std::string basedir = modelPathURI.parent_path().generic_string();

        if (!stream)
        {
            return nullptr;
        }

        String content;
        content.Resize(stream->GetSize());
        Wl::read_text(*stream, content);

        SharedPtr<GlTFAsset> model = MakeShared<GlTFAsset>();

        tinygltf::TinyGLTF loader;
        loader.SetImageLoader(DummyLoaderImage, nullptr);

        std::string err;
        std::string warn;
        bool res = loader.LoadASCIIFromString(&model->GlTFModel, &err, &warn, content.GetData(), content.GetSize(), basedir);

        if (!res)
        {
            return nullptr;
        }

        String lcaPath = PathToLCAFilepath(ctx.URI, ctx.OutputURI);
        AssetHandle handle = ctx.Registry->CreateAsset(model->AssetType, CreateSID(lcaPath));

        if (!GlTFConstruct(handle, *model, m_importers, ctx, basedir.data()))
        {
            return nullptr;
        }

        ctx.Storage.Put(handle, model);

        return model;
    }

}// namespace Wl