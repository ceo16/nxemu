#include "cpu_module.h"
#include "notification.h"

CpuModule::CpuModule() :
    m_CreateCpu(dummyCreateCpu),
    m_DestroyCpu(dummyDestroyCpu)
{
}

void CpuModule::UnloadModule(void)
{
    m_CreateCpu = nullptr;
    m_DestroyCpu = nullptr;
}

bool CpuModule::LoadFunctions(void)
{
    m_CreateCpu = (tyCreateCpu)DynamicLibraryGetProc(m_lib, "CreateCpu");
    m_DestroyCpu = (tyDestroyCpu)DynamicLibraryGetProc(m_lib, "DestroyCpu");

    bool res = true;
    if (m_CreateCpu == nullptr)
    {
        m_CreateCpu = dummyCreateCpu;
        res = false;
    }
    if (m_DestroyCpu == nullptr)
    {
        m_DestroyCpu = dummyDestroyCpu;
        res = false;
    }
    return res;
}

MODULE_TYPE CpuModule::ModuleType() const
{
    return MODULE_TYPE_CPU;
}

ICpu * CpuModule::dummyCreateCpu(ISwitchSystem & /*System*/)
{
    return nullptr;
}

void CpuModule::dummyDestroyCpu(ICpu * /*Video*/)
{
}
