#include "cpu_manager.h"

CpuManager::CpuManager(ISwitchSystem & system) :
    m_system(system)
{
}

CpuManager::~CpuManager()
{
}

bool CpuManager::Initialize(void)
{
    return true;
}

IExclusiveMonitor * CpuManager::CreateExclusiveMonitor(IMemory & /*memory*/, uint32_t /*processorCount*/)
{
    return nullptr;
};

void CpuManager::DestroyExclusiveMonitor(IExclusiveMonitor * /*monitor*/)
{
}