#include "exclusive_monitor_interface.h"

ExclusiveMonitor::ExclusiveMonitor(IMemory & /*memory*/, uint32_t processorCount) :
    Dynarmic::ExclusiveMonitor(processorCount)
{
}

uint8_t ExclusiveMonitor::ExclusiveRead8(uint32_t /*coreIndex*/, uint64_t /*addr*/)
{
    __debugbreak();
    return 0;
}

uint16_t ExclusiveMonitor::ExclusiveRead16(uint32_t /*coreIndex*/, uint64_t /*addr*/)
{
    __debugbreak();
    return 0;
}

uint32_t ExclusiveMonitor::ExclusiveRead32(uint32_t /*coreIndex*/, uint64_t /*addr*/)
{
    __debugbreak();
    return 0;
}

uint64_t ExclusiveMonitor::ExclusiveRead64(uint32_t /*coreIndex*/, uint64_t /*addr*/)
{
    __debugbreak();
    return 0;
}

void ExclusiveMonitor::ClearExclusive(uint32_t /*coreIndex*/)
{
    __debugbreak();
}

bool ExclusiveMonitor::ExclusiveWrite8(uint32_t /*coreIndex*/, uint64_t /*addr*/, uint8_t /*value*/)
{
    __debugbreak();
    return false;
}

bool ExclusiveMonitor::ExclusiveWrite16(uint32_t /*coreIndex*/, uint64_t /*addr*/, uint16_t /*value*/)
{
    __debugbreak();
    return false;
}

bool ExclusiveMonitor::ExclusiveWrite32(uint32_t /*coreIndex*/, uint64_t /*addr*/, uint32_t /*value*/)
{
    __debugbreak();
    return false;
}

bool ExclusiveMonitor::ExclusiveWrite64(uint32_t /*coreIndex*/, uint64_t /*addr*/, uint64_t /*value*/)
{
    __debugbreak();
    return false;
}