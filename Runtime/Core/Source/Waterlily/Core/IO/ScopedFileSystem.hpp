#pragma once

#include "Waterlily/Core/IO/FileSystem.hpp"

namespace Wl
{

    class ScopedFileSystem : public FileSystem
    {
    public:
        inline virtual bool FileExists(StringRef filepath) const override
        {
            return m_proxy.FileExists(Resolve(filepath));
        }

        inline virtual bool CreateDirectory(StringRef path) override
        {
            return m_proxy.CreateDirectory(Resolve(path));
        }

        inline virtual void RemoveFile(StringRef filepath) override
        {
            m_proxy.RemoveFile(Resolve(filepath));
        }

        inline virtual void Rename(StringRef path, StringRef newName) override
        {
            m_proxy.Rename(Resolve(path), newName);
        }

        inline virtual void Move(StringRef subject, StringRef toPath) override
        {
            m_proxy.Move(Resolve(subject), Resolve(toPath));
        }

        inline virtual String Extension(StringRef path) const override
        {
            return m_proxy.Extension(path);
        }

        inline virtual String NormalizePath(StringRef path) const override
        {
            return m_proxy.NormalizePath(path);
        }

        inline virtual String Filename(StringRef path) const override
        {
            return m_proxy.Filename(path);
        }

        inline virtual String Stem(StringRef path) const override
        {
            return m_proxy.Stem(path);
        }

        inline virtual String ParentPath(StringRef path) override
        {
            return m_proxy.ParentPath(path);
        }

        inline virtual FileResult Open(StringRef filepath, FileAccess access = FileAccess::ReadWrite, FileMode mode = FileMode::Open) const override
        {
            return m_proxy.Open(Resolve(filepath), access, mode);
        }

    public:
        ScopedFileSystem(FileSystem& proxy, StringRef root)
            : m_proxy(proxy)
            , m_root(root)
        {
        }

    private:
        inline String Resolve(StringRef path) const
        {
            return m_proxy.NormalizePath(m_root + "/" + String(path));
        }

    private:
        FileSystem& m_proxy;
        String m_root;
    };

}// namespace Wl
