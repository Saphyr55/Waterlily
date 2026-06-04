#include "Waterlily/Assets/assetsFileSystem.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include <filesystem>

namespace Wl
{

    bool AssetsFileSystem::FileExists(StringRef filepath) const
    {
        std::filesystem::path assetFilepath = m_assetsDirectoryPath.GetData();
        assetFilepath /= filepath.data();
        std::string assetFilepathText = assetFilepath.generic_string();
        return m_vfs.FileExists(assetFilepathText.data());
    }

    bool AssetsFileSystem::CreateDirectory(StringRef path)
    {
        return false;
    }

    void AssetsFileSystem::RemoveFile(StringRef filepath)
    {
    }

    void AssetsFileSystem::Rename(StringRef path, StringRef new_name)
    {
    }

    void AssetsFileSystem::Move(StringRef subject, StringRef to_path)
    {
    }

    String AssetsFileSystem::Extension(StringRef path) const
    {
        return "";
    }

    String AssetsFileSystem::NormalizePath(StringRef path) const
    {
        return "";
    }

    String AssetsFileSystem::Filename(StringRef path) const
    {
        return "";
    }

    String AssetsFileSystem::Stem(StringRef path) const
    {
        return "";
    }

    String AssetsFileSystem::ParentPath(StringRef path)
    {
        return "";
    }

    FileResult AssetsFileSystem::Open(StringRef filepath, FileAccess access, FileMode mode) const
    {
        return FileResult::Failure(FileError::None);
    }

}// namespace Wl
