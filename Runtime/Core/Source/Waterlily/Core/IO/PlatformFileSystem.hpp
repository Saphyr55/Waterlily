#pragma once

#include "Waterlily/Core/IO/FileSystem.hpp"

namespace Wl
{

    class WL_CORE_API PlatformFileSystem : public FileSystem
    {
    public:
        static StringRef get_current_directory();

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

        virtual FileResult Open(StringRef filepath,
                                FileAccess access = FileAccess::ReadWrite,
                                FileMode mode = FileMode::Open) const override;

        String get_file_mode(FileAccess access, FileMode mode) const;

    public:
        virtual ~PlatformFileSystem() override;
    };

}// namespace Wl