#include <nxemu-module-spec/video.h>
#include <stdio.h>

IModuleNotification * g_notify = nullptr;
IModuleSettings * g_settings = nullptr;

/*
Function: GetModuleInfo
Purpose: Fills the MODULE_INFO structure with information about the DLL.
Input: A pointer to a MODULE_INFO structure to be populated.
Output: none
*/
void CALL GetModuleInfo(MODULE_INFO * info)
{
    info->version = MODULE_VIDEO_SPECS_VERSION;
    info->type = MODULE_TYPE_VIDEO;
#ifdef _DEBUG
    sprintf(info->name, "NxEmu Video Plugin (Debug)");
#else
    sprintf(info->name, "NxEmu Video Plugin");
#endif
}

/*
Function: ModuleInitialize
Purpose: Initializes the module for global use.
Input: None
Output: Returns 0 on success
*/
int CALL ModuleInitialize(ModuleInterfaces & interfaces)
{
    g_notify = interfaces.notification;
    g_settings = interfaces.settings;

    if (g_notify == nullptr || g_settings == nullptr)
    {
        return -1;
    }
    return 0;
}

/*
Function: ModuleCleanup
Purpose: Cleans up global resources used by the module.
Input: None
Output: None
*/
void CALL ModuleCleanup()
{
}

/*
Function: EmulationStarting
Purpose: Called when emulation is starting
Input: None.
Output: None.
*/
void CALL EmulationStarting()
{
}

/*
Function: EmulationStopping
Purpose: Called when emulation is stopping
Input: None
Output: None
*/
void CALL EmulationStopping()
{
}

extern "C" int __stdcall DllMain(void * /*hinst*/, unsigned long /*fdwReason*/, void * /*lpReserved*/)
{
    return true;
}
