#include <nxemu-module-spec/operating_system.h>
#include <stdio.h>

/*
Function: GetModuleInfo
Purpose: Fills the MODULE_INFO structure with information about the DLL.
Input: A pointer to a MODULE_INFO structure to be populated.
Output: none
*/
void CALL GetModuleInfo(MODULE_INFO * info)
{
    info->version = MODULE_OPERATING_SYSTEM_SPECS_VERSION;
    info->type = MODULE_TYPE_OPERATING_SYSTEM;
#ifdef _DEBUG
    sprintf(info->name, "NxEmu Operating System Plugin (Debug)");
#else
    sprintf(info->name, "NxEmu Operating System Plugin");
#endif
}

extern "C" int __stdcall DllMain(void * /*hinst*/, unsigned long /*fdwReason*/, void * /*lpReserved*/)
{
    return true;
}
