#include <nxemu-core/settings/identifiers.h>
#include "machine/switch_system.h"
#include "settings/settings.h"
#include "switch_rom.h"

bool LaunchSwitchRom(IRenderWindow & window, const char * romFile)
{
    Settings & settings = Settings::GetInstance();
    settings.SetBool(NXCoreSetting::RomLoading, true);
    // settings on rom loading, emulation running, content rendering 
    // 
    // hide rom list, show loading screen
    // hide loading screen on first frame

    if (!SwitchSystem::Create(window))
    {
        return false;
    }
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (!system->LoadRom(romFile))
    {
        return false;
    }
    settings.SetBool(NXCoreSetting::RomLoading, false);
    settings.SetBool(NXCoreSetting::EmulationRunning, true);
    system->StartEmulation();
    return true;
}