#include "os_manager.h"

OSManager::OSManager(ISwitchSystem & switchSystem) :
    m_switchSystem(switchSystem)
{
}

OSManager::~OSManager()
{
}

void OSManager::EmulationStarting()
{
}
bool OSManager::Initialize(void)
{
    return true;
}

bool OSManager::CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress)
{
    return true;
}

void OSManager::StartApplicationProcess(uint64_t /*baseAddress*/, int32_t priority, int64_t stackSize)
{
}

bool OSManager::LoadModule(const IModuleInfo & module, uint64_t baseAddress)
{
    return true;
}

