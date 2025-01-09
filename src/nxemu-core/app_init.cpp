#include "app_init.h"
#include "notification.h"
#include "settings/core_settings.h"
#include "settings/settings.h"

bool AppInit(INotification * notification)
{
    if (!Settings::GetInstance().Initialize())
    {
        return false;
    }
    LoadCoreSetting();
    if (notification)
    {
        notification->AppInitDone();
    }
    return true;
}

void AppCleanup(void)
{
    Settings::CleanUp();
}
