#pragma once
#include "module_base.h"
#include <nxemu-module-spec/cpu.h>

class CpuModule :
    public ModuleBase
{
public:
    typedef ICpu *(CALL * tyCreateCpu)(ISwitchSystem & system);
    typedef void(CALL * tyDestroyCpu)(ICpu * Cpu);

    CpuModule();
    ~CpuModule() = default;

    ICpu * CreateCpu(ISwitchSystem & system) const
    {
        return m_createCpu(system);
    }

    void DestroyCpu(ICpu * Cpu) const
    {
        m_destroyCpu(Cpu);
    }

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);

    MODULE_TYPE ModuleType() const;

private:
    CpuModule(const CpuModule &) = delete;
    CpuModule & operator=(const CpuModule &) = delete;

    static ICpu * CALL dummyCreateCpu(ISwitchSystem & system);
    static void CALL dummyDestroyCpu(ICpu * cpu);

    tyCreateCpu m_createCpu;
    tyDestroyCpu m_destroyCpu;
};