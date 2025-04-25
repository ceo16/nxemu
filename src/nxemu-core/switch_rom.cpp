#include "switch_rom.h"
#include "machine/switch_system.h"
#include "settings/settings.h"
#include <nxemu-core/settings/identifiers.h>

bool LaunchSwitchRom(IRenderWindow & window, const char * romFile)
{
    Settings & settings = Settings::GetInstance();
    settings.SetBool(NXCoreSetting::RomLoading, true);

    if (!SwitchSystem::Create(window))
    {
        return false;
    }
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (!system->Systemloader().LoadRom(romFile))
    {
        settings.SetBool(NXCoreSetting::RomLoading, false);
        return false;
    }
    settings.SetBool(NXCoreSetting::RomLoading, false);
    settings.SetBool(NXCoreSetting::EmulationRunning, true);
    system->StartEmulation();
    return true;
}