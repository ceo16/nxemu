#pragma once
#include <common/dynamic_library.h>

#ifndef EXPORT
#define EXPORT
#endif
#include <nxemu-module-spec/base.h>

class ModuleList;

class ModuleBase
{
    friend ModuleList;

    typedef void(CALL * tyGetModuleInfo)(MODULE_INFO * info);

public:
    ModuleBase();

protected:
    static bool ValidVersion(MODULE_INFO & info);

private:
    ModuleBase(const ModuleBase &) = delete;
    ModuleBase & operator=(const ModuleBase &) = delete;
};