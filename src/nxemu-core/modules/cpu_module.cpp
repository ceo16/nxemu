#include "cpu_module.h"

void CpuModule::UnloadModule(void)
{
}

bool CpuModule::LoadFunctions(void)
{
    return true;
}

MODULE_TYPE CpuModule::ModuleType() const
{
    return MODULE_TYPE_CPU;
}
