#pragma once
#include <vector>
#include "yuzu_common/interface_pointer.h"

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
