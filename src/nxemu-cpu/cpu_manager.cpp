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

IArm64Executor * CpuManager::CreateArm64Executor(IExclusiveMonitor * monitor, ICpuInfo & info, uint32_t coreIndex)
{
    return nullptr;
}

void CpuManager::DestroyArm64Executor(IArm64Executor * executor)
{
}
