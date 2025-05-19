#include "filesystem_interfaces.h"
#include <nxemu-module-spec/system_loader.h>

IVirtualDirectoryPtr::IVirtualDirectoryPtr() :
    m_directory(nullptr)
{
}

IVirtualDirectoryPtr::IVirtualDirectoryPtr(IVirtualDirectory * ptr) :
    m_directory(ptr)
{
}

IVirtualDirectoryPtr::~IVirtualDirectoryPtr()
{
    if (m_directory != nullptr)
    {
        m_directory->Release();
    }
    m_directory = nullptr;
}

IVirtualDirectoryPtr & IVirtualDirectoryPtr::operator=(IVirtualDirectory * ptr)
{
    if (m_directory != nullptr)
    {
        m_directory->Release();
    }
    m_directory = ptr;
    return *this;
}

IVirtualDirectoryPtr& IVirtualDirectoryPtr::operator=(IVirtualDirectoryPtr && other) noexcept
{
    if (this != &other)
    {
        if (m_directory)
        {
            m_directory->Release();
        }
        m_directory = other.m_directory;
        other.m_directory = nullptr;
    }
    return *this;
}

IVirtualDirectory ** IVirtualDirectoryPtr::GetAddressForSet()
{
    if (m_directory)
    {
        m_directory->Release();
        m_directory = nullptr;
    }
    return &m_directory;
}

IVirtualDirectory * IVirtualDirectoryPtr::operator->() const
{
    return m_directory;
}

IVirtualDirectory & IVirtualDirectoryPtr::operator*() const
{
    return *m_directory;
}

IVirtualDirectoryPtr::operator bool() const
{
    return m_directory != nullptr;
}

IVirtualFilePtr::IVirtualFilePtr() :
    m_file(nullptr)
{
}

IVirtualFilePtr::IVirtualFilePtr(IVirtualFile * file) :
    m_file(file)
{
}

IVirtualFilePtr::IVirtualFilePtr(IVirtualFilePtr && other) noexcept
{
    m_file = other.m_file;
    other.m_file = nullptr;
}

IVirtualFilePtr::~IVirtualFilePtr()
{
    if (m_file != nullptr)
    {
        m_file->Release();
    }
    m_file = nullptr;
}

IVirtualFilePtr & IVirtualFilePtr::operator=(IVirtualFile * ptr)
{
    if (m_file != ptr && m_file != nullptr)
    {
        m_file->Release();
    }
    m_file = ptr;
    return *this;
}

IVirtualFilePtr & IVirtualFilePtr::operator=(IVirtualFilePtr && other) noexcept
{
    if (m_file != nullptr)
    {
        m_file->Release();
    }
    m_file = other.m_file;
    other.m_file = nullptr;
    return *this;
}

IVirtualFile * IVirtualFilePtr::operator->() const
{
    return m_file;
}

IVirtualFile & IVirtualFilePtr::operator*() const
{
    return *m_file;
}

IVirtualFilePtr::operator bool() const
{
    return m_file != nullptr;
}

std::vector<uint8_t> IVirtualFilePtr::ReadAllBytes() const
{
    uint64_t dataSize = m_file->GetSize();
    if (dataSize == 0)
    {
        return {};
    }
    std::vector<uint8_t> data;
    data.resize(dataSize);
    dataSize = m_file->ReadBytes(data.data(), data.size());
    if (dataSize < data.size())
    {
        data.resize(dataSize);
    }
    return data;
}

FileSysNCAPtr::FileSysNCAPtr() :
    m_ptr(nullptr)
{
}

FileSysNCAPtr::FileSysNCAPtr(IFileSysNCA* ptr) :
    m_ptr(ptr) 
{
}

FileSysNCAPtr::FileSysNCAPtr(FileSysNCAPtr&& other) noexcept : 
    m_ptr(other.m_ptr) 
{
    other.m_ptr = nullptr;
}

FileSysNCAPtr::~FileSysNCAPtr() 
{
    if (m_ptr)
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

FileSysNCAPtr& FileSysNCAPtr::operator=(FileSysNCAPtr&& other) noexcept 
{
    if (this != &other) 
    {
        if (m_ptr)
        {
            m_ptr->Release();
        }
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
    }
    return *this;
}

IFileSysNCA* FileSysNCAPtr::operator->() const 
{
    return m_ptr;
}

IFileSysNCA& FileSysNCAPtr::operator*() const 
{
    return *m_ptr;
}

FileSysNCAPtr::operator bool() const 
{
    return m_ptr != nullptr;
}

SaveDataFactoryPtr::SaveDataFactoryPtr() :
    m_ptr(nullptr)
{
}

SaveDataFactoryPtr::SaveDataFactoryPtr(SaveDataFactoryPtr && other) noexcept
{
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
}

SaveDataFactoryPtr::~SaveDataFactoryPtr()
{
    if (m_ptr)
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

SaveDataFactoryPtr & SaveDataFactoryPtr::operator=(SaveDataFactoryPtr&& other) noexcept
{
    if (m_ptr)
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
}

ISaveDataFactory ** SaveDataFactoryPtr::GetAddressForSet()
{
    if (m_ptr) 
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
    return &m_ptr;
}

RomFsControllerPtr::RomFsControllerPtr() :
    m_ptr(nullptr)
{
}

RomFsControllerPtr::RomFsControllerPtr(RomFsControllerPtr && other) noexcept
{
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
}


RomFsControllerPtr::~RomFsControllerPtr()
{
    if (m_ptr)
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

IRomFsController ** RomFsControllerPtr::GetAddressForSet()
{
    if (m_ptr)
    {
        m_ptr->Release();
        m_ptr = nullptr;
    }
    return &m_ptr;
}

IRomFsController * RomFsControllerPtr::operator->() const
{
    return m_ptr;
}
