#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

using namespace Wl;


static std::string to_hex64(uint64_t v)
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(16) << std::setfill('0') << v;
    return oss.str();
}

static std::string decode_filetype(uint64_t v)
{
    std::string s(8, '\0');
    for (int i = 0; i < 8; i++)
    {
        s[i] = char((v >> (i * 8)) & 0xFF);
    }
    return s;
}

static void dump_header(auto& out, FileHandle& file)
{
    LARHeader header;
    file >> header;

    std::string filetype = decode_filetype(header.Filetype);
    std::string filetype_hex = to_hex64(header.Filetype);

    out << "================ LAR DUMP ================\n";
    out << "[HEADER]\n";
    out << "  Filetype     = ";
    out << filetype.data();
    out << " (";
    out << filetype_hex.data();
    out << ")\n";
    out << "  Version      = ";
    out << header.Version;
    out << "\n";
    out << "  Asset count  = ";
    out << header.AssetCount;
    out << "\n";
    out << "  Asset offset = ";
    out << header.AssetOffset;
    out << "\n\n";

    file.Seek(header.AssetOffset);
}

static void dump_registry(auto& out, const AssetRegistry& registry)
{
    out << StringRef("[ASSETS]\n");

    for (AssetMetadata& meta: registry.GetRegistry())
    {
        String uuid_text = meta.GetUUID().ToString();

        out << "  ----------------------------------------\n";
        out << "  Asset ";
        out << uuid_text;
        out << "\n";
        out << "    URI        = ";
        out << meta.GetURI().GetText().data();
        out << "\n";
        out << "    URI_Hash   = ";
        out << meta.GetURI().GetHash();
        out << "\n";
        out << "    Type       = ";
        out << meta.GetAssetType().GetText().data();
        out << "\n";
        out << "    Type_Hash  = ";
        out << meta.GetAssetType().GetHash();
        out << "\n";
        out << "    UUID       = ";
        out << uuid_text;
        out << "\n";
        out << "    UUID_Value = ";
        out << meta.GetUUID().GetValue();
        out << "\n";
        out << "    Deps     = [";

        if (meta.GetDependencies().IsEmpty())
        {
            out << "]\n";
        }
        else
        {
            out << "\n";
            for (AssetUUID& dep: meta.GetDependencies())
            {
                out << "      ";
                out << dep.ToString();
                out << ",\n";
            }
            out << "    ]\n";
        }
        out << "  ----------------------------------------\n";
    }
}

void dump_lar_file(auto& out, FileHandle& file)
{
    auto registry = AssetRegistry::LoadFromFile(file);

    dump_header(out, file);
    dump_registry(out, *registry);
}

int32_t main(int32_t argc, const char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: lichtacp-dump <file.lar> <output.txt>\n";
        return EXIT_FAILURE;
    }

    StringRef filepath = argv[1];
    StringRef output = argv[2];

    FileSystem& fs = FileSystem::GetPlatform();

    LLOG_INFO("[LARDump]", Wl::Format("Opening file: \"%s\"", filepath.data()));

    FileResult file_result = fs.Open(filepath);
    if (!file_result.HasValue())
    {
        LLOG_ERROR("[LARDump]", "Failed to open the file.");
        return EXIT_FAILURE;
    }

    std::ofstream output_file;
    output_file.open(output, std::ios::out | std::ios::trunc);

    dump_lar_file(output_file, *file_result.GetValue());

    return EXIT_SUCCESS;
}
