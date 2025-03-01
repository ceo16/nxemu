#include "switch_rom.h"
#include "machine/switch_system.h"

bool LaunchSwitchRom(IRenderWindow & window, const char * romFile)
{
    if (!SwitchSystem::Create(window))
    {
        return false;
    }
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (!system->LoadRom(romFile))
    {
        return false;
    }
    system->StartEmulation();
    return true;
}