#include "switch_rom.h"
#include "settings/settings.h"

bool LaunchSwitchRom(const char * switchRomFile)
{
    Settings::GetInstance().SetString("nxcore:gamefile", switchRomFile);
    return true;
}