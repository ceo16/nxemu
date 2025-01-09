#include "module_base.h"

ModuleBase::ModuleBase()
{
}

bool ModuleBase::ValidVersion(MODULE_INFO & info)
{
    if (info.type == MODULE_TYPE_CPU && info.version == MODULE_CPU_SPECS_VERSION)
    {
        return true;
    }
    if (info.type == MODULE_TYPE_VIDEO && info.version == MODULE_VIDEO_SPECS_VERSION)
    {
        return true;
    }
    if (info.type == MODULE_TYPE_OPERATING_SYSTEM && info.version == MODULE_OPERATING_SYSTEM_SPECS_VERSION)
    {
        return true;
    }
    return false;
}