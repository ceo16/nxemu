#include "vfs_types.h"
#include "core/file_sys/romfs.h"

VirtualDirectoryPtr::VirtualDirectoryPtr(FileSys::VirtualDir & directory) :
    m_directory(directory)
{
}

VirtualDirectoryPtr::~VirtualDirectoryPtr()
{
}

IVirtualFile * VirtualDirectoryPtr::GetFile(const char * name) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualFile file = m_directory->GetFile(name);
    if (file.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualFilePtr>(file).release();
}

IVirtualFile* VirtualDirectoryPtr::GetFileRelative(const char* name) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualFile file = m_directory->GetFileRelative(name);
    if (file.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualFilePtr>(file).release();
}

void VirtualDirectoryPtr::Release()
{
    delete this;
}

VirtualFilePtr::VirtualFilePtr(FileSys::VirtualFile & file) :
    m_file(file)
{
}

VirtualFilePtr::~VirtualFilePtr()
{
}

uint64_t VirtualFilePtr::GetSize() const
{
    if (m_file)
    {
        return m_file->GetSize();
    }
    return 0;
}

uint64_t VirtualFilePtr::ReadBytes(uint8_t * data, uint64_t datalen)
{
    if (m_file)
    {
        return m_file->Read(data, datalen);
    }
    return 0;
}

IVirtualDirectory * VirtualFilePtr::ExtractRomFS()
{
    FileSys::VirtualDir dir = FileSys::ExtractRomFS(m_file);
    if (dir.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualDirectoryPtr>(dir).release();
}

void VirtualFilePtr::Release()
{
    delete this;
}