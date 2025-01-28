#pragma once

#include "module_base.h"

class ModuleSettings :
    public IModuleSettings
{
public:
    //IModuleSettings
    std::string GetString(const char * setting) const;
    bool GetBool(const char * setting) const;
    void SetString(const char * setting, const char * value);
    void SetBool(const char * setting, bool value);
};