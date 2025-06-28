#include "app_init.h"
#include "machine/switch_system.h"
#include "notification.h"
#include "settings/core_settings.h"
#include "settings/settings.h"

extern "C" int __stdcall AllocConsole();

bool AppInit(INotification * notification)
{
    g_notify = notification;
    if (!SettingsStore::GetInstance().Initialize())
    {
        return false;
    }
    LoadCoreSetting();
    if (coreSettings.showConsole)
    {
        if (AllocConsole())
        {
            FILE * fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            freopen_s(&fp, "CONIN$", "r", stdin);
        }
    }

    if (notification)
    {
        notification->AppInitDone();
    }
    return true;
}

void AppCleanup(void)
{
    SwitchSystem::ShutDown();
    SettingsStore::CleanUp();
}
