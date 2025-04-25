#pragma once
#include "base.h"

__interface ISystemloader
{
    bool SelectAndLoad(void * parentWindow) = 0;
    bool LoadRom(const char * romFile) = 0;
};

EXPORT ISystemloader * CALL CreateSystemLoader(ISwitchSystem & system);
EXPORT void CALL DestroySystemLoader(ISystemloader * systemloader);
