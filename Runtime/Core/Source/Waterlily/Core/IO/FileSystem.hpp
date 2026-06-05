#pragma once

#include "Waterlily/Core/Containers/Result.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class FileHandle;

    enum class FileError
    {
        None,
        NotFound,
        AlreadyExists,
        AccessDenied,
        InvalidArgument,
        Unknown,
    };

    enum class FileAccess
    {
        Read,
        Write,
        ReadWrite,
    };

    enum class FileMode
    {
        /**
         * Opens an existing file.
         * Fails if the file does not exist.
         */
        Open,

        /**
         * Creates a file.
         * If the file already exists, its contents are typically overwritten.
         */
        Create,

        /**
         * Creates a new file only.
         * Fails if the file already exists.
         */
        CreateNew,

        /**
         * Opens the file if it exists, otherwise creates it.
         */
        OpenOrCreate,

        /**
         * Opens the file and writes at the end.
         * Creates the file if it does not exist.
         */
        Append,
    };

    using FileResult = Result<SharedPtr<FileHandle>, FileError>;

    class WL_CORE_API FileSystem
    {
    public:
        static FileSystem& GetPlatform();

        virtual bool FileExists(StringRef filepath) const = 0;

        virtual bool CreateDirectory(StringRef path) = 0;

        virtual void RemoveFile(StringRef filepath) = 0;

        virtual void Rename(StringRef path, StringRef newName) = 0;

        virtual void Move(StringRef subject, StringRef toPath) = 0;

        virtual String Extension(StringRef path) const = 0;

        virtual String NormalizePath(StringRef path) const = 0;

        virtual String Filename(StringRef path) const = 0;

        virtual String Stem(StringRef path) const = 0;

        virtual String ParentPath(StringRef path) = 0;

        virtual FileResult Open(StringRef filepath, FileAccess access = FileAccess::ReadWrite, FileMode mode = FileMode::Open) const = 0;

        FileResult OpenWrite(StringRef filepath, FileMode mode = FileMode::Open) const
        {
            return Open(filepath, FileAccess::Write, mode);
        }

        FileResult OpenRead(StringRef filepath, FileMode mode = FileMode::Open) const
        {
            return Open(filepath, FileAccess::Read, mode);
        }

    public:
        virtual ~FileSystem() = default;
    };

}// namespace Wl