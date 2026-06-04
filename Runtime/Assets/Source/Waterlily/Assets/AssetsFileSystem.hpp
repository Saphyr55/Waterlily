#pragma once

#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/String/String.hpp"

namespace Wl
{

    class AssetsFileSystem : public FileSystem
    {
    public:
        virtual bool FileExists(StringRef filepath) const override;

        virtual bool CreateDirectory(StringRef path) override;

        virtual void RemoveFile(StringRef filepath) override;

        virtual void Rename(StringRef path, StringRef newName) override;

        virtual void Move(StringRef subject, StringRef toPath) override;

        virtual String Extension(StringRef path) const override;

        virtual String NormalizePath(StringRef path) const override;

        virtual String Filename(StringRef path) const override;

        virtual String Stem(StringRef path) const override;

        virtual String ParentPath(StringRef path) override;

        virtual FileResult Open(StringRef filepath, FileAccess access = FileAccess::ReadWrite, FileMode mode = FileMode::Open) const override;

    public:
        AssetsFileSystem(StringRef assetsDirectoryPath)
            : m_assetsDirectoryPath(assetsDirectoryPath)
            , m_vfs(FileSystem::GetPlatform())
        {
        }

    private:
        String m_assetsDirectoryPath;
        FileSystem& m_vfs;
    };

}// namespace Wl
