#pragma once
#include "base.h"
#include <stdint.h>

__interface IVideo
{
    bool Initialize(void) = 0;
};

EXPORT IVideo * CALL CreateVideo(IRenderWindow & RenderWindow, ISwitchSystem & System);
EXPORT void CALL DestroyVideo(IVideo * Video);
