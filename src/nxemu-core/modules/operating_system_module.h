#pragma once
#include "module_base.h"

class OperatingSystemModule :
    public ModuleBase
{
public:
    OperatingSystemModule() = default;
    ~OperatingSystemModule() = default;

protected:
    void UnloadModule();
    bool LoadFunctions();
    MODULE_TYPE ModuleType() const;

private:
    OperatingSystemModule(const OperatingSystemModule &) = delete;
    OperatingSystemModule & operator=(const OperatingSystemModule &) = delete;
};