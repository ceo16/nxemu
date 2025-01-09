#include "switch_rom.h"
#include "settings/settings.h"
#include <nxemu-core/settings/core_settings.h>

bool LaunchSwitchRom(const char * switchRomFile)
{
    Settings::GetInstance().SetString(NXCoreSetting::GameFile, switchRomFile);
    return true;
}