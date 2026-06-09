#include "Waterlily/Core/IO/PlatformFileSystem.hpp"
#include "Waterlily/Core/IO/PlatformFile.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/PlatformFileSystem.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <cstdio>
#include <filesystem>

#ifndef _MSC_VER
#include <sys/stat.h>
#endif

namespace Wl
{

    FileSystem& FileSystem::GetPlatform()
    {
        static PlatformFileSystem s_platformFileSystem;
        return s_platformFileSystem;
    }

    StringRef PlatformFileSystem::get_current_directory()
    {
        return PlatformGetCurrentDirectory();
    }

    bool PlatformFileSystem::FileExists(StringRef filepath) const
    {
#ifdef _MSC_VER
        struct _stat buffer
        {
        };

        return _stat(filepath, &buffer) == 0;
#else
        struct stat buffer
        {
        };
        return stat(filepath, &buffer) == 0;
#endif
    }

    bool PlatformFileSystem::CreateDirectory(StringRef path)
    {
        std::error_code error;
        std::filesystem::create_directories(path.data(), error);
        return !error;
    }

    void PlatformFileSystem::RemoveFile(StringRef filepath)
    {
    }

    void PlatformFileSystem::Rename(StringRef path, StringRef newName)
    {
    }

    void PlatformFileSystem::Move(StringRef subject, StringRef toPath)
    {
    }

    String PlatformFileSystem::Extension(StringRef path) const
    {
        std::filesystem::path fsPath(path.data());
        return fsPath.extension().string().c_str();
    }

    String PlatformFileSystem::NormalizePath(StringRef path) const
    {
        std::filesystem::path fsPath(path.data());
        return fsPath.lexically_normal().string().c_str();
    }

    String PlatformFileSystem::Filename(StringRef path) const
    {
        std::filesystem::path fsPath(path.data());
        return fsPath.filename().string().c_str();
    }

    String PlatformFileSystem::Stem(StringRef path) const
    {
        std::filesystem::path fsPath(path.data());
        return fsPath.stem().string().c_str();
    }

    String PlatformFileSystem::ParentPath(StringRef path)
    {
        std::filesystem::path fsPath(path.data());
        return fsPath.parent_path().string().c_str();
    }

    FileResult PlatformFileSystem::Open(StringRef filepath, FileAccess access, FileMode mode) const
    {
        FILE* stream;

        String modeStr = get_file_mode(access, mode);

        modeStr.Append('b');// Open in binary mode.

        errno_t error = fopen_s(&stream, filepath, modeStr.GetData());

        if (!stream)
        {
            return FileResult::Failure(FileError::Unknown);
        }

        SharedPtr<File> file = MakeShared<PlatformFile>(stream);

        return FileResult::Success(file);
    }

    String PlatformFileSystem::get_file_mode(FileAccess access, FileMode mode) const
    {
        String modeStr;
        switch (mode)
        {
            case FileMode::Open:
                switch (access)
                {
                    case FileAccess::Read:
                        modeStr = "r";
                        break;
                    case FileAccess::Write:
                        modeStr = "r+";
                        break;
                    case FileAccess::ReadWrite:
                        modeStr = "r+";
                        break;
                }
                break;

            case FileMode::Create:
                switch (access)
                {
                    case FileAccess::Read:
                        modeStr = "w+";
                        break;
                    case FileAccess::Write:
                        modeStr = "w";
                        break;
                    case FileAccess::ReadWrite:
                        modeStr = "w+";
                        break;
                }
                break;

            case FileMode::CreateNew:
                switch (access)
                {
                    case FileAccess::Read:
                        modeStr = "w+";
                        break;
                    case FileAccess::Write:
                        modeStr = "w";
                        break;
                    case FileAccess::ReadWrite:
                        modeStr = "w+";
                        break;
                }
                break;

            case FileMode::OpenOrCreate:
                switch (access)
                {
                    case FileAccess::Read:
                        modeStr = "a+";
                        break;
                    case FileAccess::Write:
                        modeStr = "a";
                        break;
                    case FileAccess::ReadWrite:
                        modeStr = "a+";
                        break;
                }
                break;

            case FileMode::Append:
                switch (access)
                {
                    case FileAccess::Read:
                        modeStr = "a+";
                        break;
                    case FileAccess::Write:
                        modeStr = "a";
                        break;
                    case FileAccess::ReadWrite:
                        modeStr = "a+";
                        break;
                }
                break;
        }
        return modeStr;
    }

    PlatformFileSystem::~PlatformFileSystem()
    {
    }

}// namespace Wl