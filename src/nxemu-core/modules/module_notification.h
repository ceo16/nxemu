#pragma once

#include "module_base.h"

class ModuleNotification :
    public IModuleNotification
{
public:
    void DisplayError(const char * message);
    void BreakPoint(const char * fileName, uint32_t lineNumber);
};