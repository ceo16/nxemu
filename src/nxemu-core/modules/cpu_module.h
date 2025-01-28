#pragma once
#include "module_base.h"

class CpuModule :
    public ModuleBase
{
public:
    CpuModule() = default;
    ~CpuModule() = default;

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);

    MODULE_TYPE ModuleType() const;

private:
    CpuModule(const CpuModule &) = delete;
    CpuModule & operator=(const CpuModule &) = delete;
};