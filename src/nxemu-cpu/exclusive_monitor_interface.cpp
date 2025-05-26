#include "exclusive_monitor_interface.h"

ExclusiveMonitor::ExclusiveMonitor(IMemory & memory, uint32_t processorCount) :
    Dynarmic::ExclusiveMonitor(processorCount),
    m_memory(memory)
{
}

uint8_t ExclusiveMonitor::ExclusiveRead8(uint32_t coreIndex, uint64_t addr)
{
    return ReadAndMark<uint8_t>(coreIndex, addr, [&]() -> uint8_t { return m_memory.Read8(addr); });
}

uint16_t ExclusiveMonitor::ExclusiveRead16(uint32_t coreIndex, uint64_t addr)
{
    return ReadAndMark<uint16_t>(coreIndex, addr, [&]() -> uint16_t { return m_memory.Read16(addr); });
}

uint32_t ExclusiveMonitor::ExclusiveRead32(uint32_t coreIndex, uint64_t addr)
{
    return ReadAndMark<uint32_t>(coreIndex, addr, [&]() -> uint32_t { return m_memory.Read32(addr); });
}

uint64_t ExclusiveMonitor::ExclusiveRead64(uint32_t coreIndex, uint64_t addr)
{
    return ReadAndMark<uint64_t>(coreIndex, addr, [&]() -> uint64_t { return m_memory.Read64(addr); });
}

void ExclusiveMonitor::ClearExclusive(uint32_t coreIndex)
{
    ClearProcessor(coreIndex);
}

bool ExclusiveMonitor::ExclusiveWrite8(uint32_t coreIndex, uint64_t addr, uint8_t value)
{
    return DoExclusiveOperation<uint8_t>(coreIndex, addr, [&](uint8_t expected) -> bool {
        return m_memory.WriteExclusive8(addr, value, expected);
    });
}

bool ExclusiveMonitor::ExclusiveWrite16(uint32_t coreIndex, uint64_t addr, uint16_t value)
{
    return DoExclusiveOperation<uint16_t>(coreIndex, addr, [&](uint16_t expected) -> bool {
        return m_memory.WriteExclusive16(addr, value, expected);
    });
}

bool ExclusiveMonitor::ExclusiveWrite32(uint32_t coreIndex, uint64_t addr, uint32_t value)
{
    return DoExclusiveOperation<uint32_t>(coreIndex, addr, [&](uint32_t expected) -> bool {
        return m_memory.WriteExclusive32(addr, value, expected);
    });
}

bool ExclusiveMonitor::ExclusiveWrite64(uint32_t coreIndex, uint64_t addr, uint64_t value)
{
    return DoExclusiveOperation<uint64_t>(coreIndex, addr, [&](uint64_t expected) -> bool {
        return m_memory.WriteExclusive64(addr   , value, expected);
    });
}