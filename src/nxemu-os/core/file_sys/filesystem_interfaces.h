#pragma once
#include <vector>

__interface IVirtualDirectory;

class IVirtualDirectoryPtr
{
public:
    IVirtualDirectoryPtr();
    IVirtualDirectoryPtr(IVirtualDirectory *);
    IVirtualDirectoryPtr(IVirtualDirectoryPtr && other) noexcept;
    ~IVirtualDirectoryPtr();

    IVirtualDirectoryPtr & operator=(IVirtualDirectory * ptr);
    IVirtualDirectoryPtr & operator=(IVirtualDirectoryPtr && other) noexcept;
    IVirtualDirectory ** GetAddressForSet();

    IVirtualDirectory * operator->() const;
    IVirtualDirectory & operator*() const;
    operator bool() const;

private:
    IVirtualDirectoryPtr(const IVirtualDirectoryPtr &) = delete;
    IVirtualDirectoryPtr & operator=(const IVirtualDirectoryPtr &) = delete;

    IVirtualDirectory * m_directory;
};

__interface IVirtualFile;

class IVirtualFilePtr
{
public:
    IVirtualFilePtr();
    IVirtualFilePtr(IVirtualFile*);
    IVirtualFilePtr(IVirtualFilePtr&& other) noexcept;
    ~IVirtualFilePtr();

    IVirtualFilePtr & operator=(IVirtualFile * ptr);
    IVirtualFilePtr & operator=(IVirtualFilePtr && other) noexcept;

    IVirtualFile * operator->() const;
    IVirtualFile & operator*() const;
    operator bool() const;

    std::vector<uint8_t> ReadAllBytes() const;

private:
    IVirtualFilePtr(const IVirtualFilePtr &) = delete;
    IVirtualFilePtr & operator=(const IVirtualFilePtr &) = delete;

    IVirtualFile * m_file;
};

__interface IFileSysNCA;

class FileSysNCAPtr
{
public:
    FileSysNCAPtr();
    FileSysNCAPtr(IFileSysNCA * ptr);
    FileSysNCAPtr(FileSysNCAPtr && other) noexcept;
    ~FileSysNCAPtr();

    FileSysNCAPtr& operator=(FileSysNCAPtr&& other) noexcept;

    IFileSysNCA * operator->() const;
    IFileSysNCA & operator*() const;
    operator bool() const;

private:
    FileSysNCAPtr(const FileSysNCAPtr&) = delete;
    FileSysNCAPtr& operator=(const FileSysNCAPtr&) = delete;

    IFileSysNCA * m_ptr;
};

__interface ISaveDataFactory;

class SaveDataFactoryPtr
{
public:
    SaveDataFactoryPtr();
    SaveDataFactoryPtr(ISaveDataFactory * ptr);
    SaveDataFactoryPtr(SaveDataFactoryPtr && other) noexcept;
    ~SaveDataFactoryPtr();

    SaveDataFactoryPtr& operator=(SaveDataFactoryPtr&& other) noexcept;
    ISaveDataFactory ** GetAddressForSet();

    ISaveDataFactory* operator->() const;
    ISaveDataFactory& operator*() const;
    operator bool() const;

private:
    SaveDataFactoryPtr(const SaveDataFactoryPtr&) = delete;
    SaveDataFactoryPtr& operator=(const SaveDataFactoryPtr&) = delete;

    ISaveDataFactory * m_ptr;
};

__interface IRomFsController;

class RomFsControllerPtr
{
public:
    RomFsControllerPtr();
    RomFsControllerPtr(IFileSysNCA * ptr);
    RomFsControllerPtr(RomFsControllerPtr && other) noexcept;
    ~RomFsControllerPtr();

    RomFsControllerPtr & operator=(RomFsControllerPtr && other) noexcept;
    IRomFsController ** GetAddressForSet();

    IRomFsController * operator->() const;
    IRomFsController & operator*() const;
    operator bool() const;

private:
    RomFsControllerPtr(const RomFsControllerPtr &) = delete;
    RomFsControllerPtr & operator=(const RomFsControllerPtr &) = delete;

    IRomFsController * m_ptr;
};
