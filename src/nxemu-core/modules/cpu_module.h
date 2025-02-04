#pragma once
#include "module_base.h"
#include <nxemu-module-spec/cpu.h>

class CpuModule :
    public ModuleBase
{
public:
    typedef ICpu *(CALL * tyCreateCpu)(ISwitchSystem & System);
    typedef void(CALL * tyDestroyCpu)(ICpu * Cpu);

    CpuModule();
    ~CpuModule() = default;

    ICpu * CreateCpu(ISwitchSystem & System) const
    {
        return m_CreateCpu(System);
    }

    void DestroyCpu(ICpu * Cpu) const
    {
        m_DestroyCpu(Cpu);
    }

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);

    MODULE_TYPE ModuleType() const;

private:
    CpuModule(const CpuModule &) = delete;
    CpuModule & operator=(const CpuModule &) = delete;

    static ICpu * CALL dummyCreateCpu(ISwitchSystem & System);
    static void CALL dummyDestroyCpu(ICpu * Video);

    tyCreateCpu m_CreateCpu;
    tyDestroyCpu m_DestroyCpu;
};