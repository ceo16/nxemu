#include "app_init.h"
#include "settings/settings.h"
#include "notification.h"

bool AppInit(INotification * notification)
{
    if (!Settings::GetInstance().Initialize())
    {
        return false;
    }
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
