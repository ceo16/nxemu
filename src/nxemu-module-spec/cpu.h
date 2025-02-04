#pragma once
#include "base.h"

__interface IMemory
{
};

__interface IExclusiveMonitor
{
    uint8_t ExclusiveRead8(uint32_t coreIndex, uint64_t addr) = 0;
    uint16_t ExclusiveRead16(uint32_t coreIndex, uint64_t addr) = 0;
    uint32_t ExclusiveRead32(uint32_t coreIndex, uint64_t addr) = 0;
    uint64_t ExclusiveRead64(uint32_t coreIndex, uint64_t addr) = 0;
    void ClearExclusive(uint32_t coreIndex) = 0;

    bool ExclusiveWrite8(uint32_t coreIndex, uint64_t vaddr, uint8_t value) = 0;
    bool ExclusiveWrite16(uint32_t coreIndex, uint64_t vaddr, uint16_t value) = 0;
    bool ExclusiveWrite32(uint32_t coreIndex, uint64_t vaddr, uint32_t value) = 0;
    bool ExclusiveWrite64(uint32_t coreIndex, uint64_t vaddr, uint64_t value) = 0;
};

__interface ICpu
{
    bool Initialize(void) = 0;

    IExclusiveMonitor * CreateExclusiveMonitor(IMemory & memory, uint32_t processorCount) = 0;
    void DestroyExclusiveMonitor(IExclusiveMonitor * monitor) = 0;
};

EXPORT ICpu * CALL CreateCpu(ISwitchSystem & System);
EXPORT void CALL DestroyCpu(ICpu * Cpu);
