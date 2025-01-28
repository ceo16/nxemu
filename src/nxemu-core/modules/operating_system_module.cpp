#include "operating_system_module.h"

void OperatingSystemModule::UnloadModule()
{
}

bool OperatingSystemModule::LoadFunctions()
{
    return true;
}

MODULE_TYPE OperatingSystemModule::ModuleType() const
{
    return MODULE_TYPE_OPERATING_SYSTEM;
}
