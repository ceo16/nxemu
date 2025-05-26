#pragma once
#include <vector>

template<typename InterfaceType>
class InterfacePtr
{
public:
    InterfacePtr();
    InterfacePtr(InterfaceType *);
    InterfacePtr(InterfacePtr && other) noexcept;
    ~InterfacePtr();

    InterfacePtr & operator=(InterfaceType * ptr);
    InterfacePtr & operator=(InterfacePtr && other) noexcept;
    InterfaceType ** GetAddressForSet();

    InterfaceType * operator->() const;
    InterfaceType & operator*() const;
    operator bool() const;

protected:
    InterfacePtr(const InterfacePtr &) = delete;
    InterfacePtr & operator=(const InterfacePtr &) = delete;

    InterfaceType * m_ptr;
};

__interface IVirtualDirectory;
__interface IVirtualFile;
__interface IFileSysNCA;
__interface ISaveDataFactory;
__interface IRomFsController;
__interface IFileSysNACP;

using IVirtualDirectoryPtr = InterfacePtr<IVirtualDirectory>;
using FileSysNCAPtr = InterfacePtr<IFileSysNCA>;
using SaveDataFactoryPtr = InterfacePtr<ISaveDataFactory>;
using RomFsControllerPtr = InterfacePtr<IRomFsController>;
using IFileSysNACPPtr = InterfacePtr<IFileSysNACP>;

class IVirtualFilePtr : public InterfacePtr<IVirtualFile>
{
public:
    std::vector<uint8_t> ReadAllBytes() const;
};
