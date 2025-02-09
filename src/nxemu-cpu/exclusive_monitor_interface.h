#pragma once
#include <nxemu-module-spec/cpu.h>
#include "dynarmic/interface/exclusive_monitor.h"

__interface IMemory;

class ExclusiveMonitor :
    public Dynarmic::ExclusiveMonitor,
    public IExclusiveMonitor
{
public:
    ExclusiveMonitor(IMemory & memory, uint32_t processorCount);

    // IExclusiveMonitor
    uint8_t ExclusiveRead8(uint32_t coreIndex, uint64_t addr);
    uint16_t ExclusiveRead16(uint32_t coreIndex, uint64_t addr);
    uint32_t ExclusiveRead32(uint32_t coreIndex, uint64_t addr);
    uint64_t ExclusiveRead64(uint32_t coreIndex, uint64_t addr);
    void ClearExclusive(uint32_t coreIndex);

    bool ExclusiveWrite8(uint32_t coreIndex, uint64_t vaddr, uint8_t value);
    bool ExclusiveWrite16(uint32_t coreIndex, uint64_t vaddr, uint16_t value);
    bool ExclusiveWrite32(uint32_t coreIndex, uint64_t vaddr, uint32_t value);
    bool ExclusiveWrite64(uint32_t coreIndex, uint64_t vaddr, uint64_t value);

private:
    ExclusiveMonitor() = delete;
    ExclusiveMonitor(const ExclusiveMonitor &) = delete;
    ExclusiveMonitor & operator=(const ExclusiveMonitor &) = delete;
};