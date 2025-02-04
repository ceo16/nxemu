#pragma once
#include <nxemu-module-spec/cpu.h>
#include <memory>

class ExclusiveMonitor;

class CpuManager :
    public ICpu
{
public:
    CpuManager(ISwitchSystem & system);
    ~CpuManager();

    //ICpu
    bool Initialize(void);
    IExclusiveMonitor * CreateExclusiveMonitor(IMemory & memory, uint32_t processorCount);
    void DestroyExclusiveMonitor(IExclusiveMonitor * monitor);

private:
    CpuManager() = delete;
    CpuManager(const CpuManager &) = delete;
    CpuManager & operator=(const CpuManager &) = delete;

    ISwitchSystem & m_system;
};
