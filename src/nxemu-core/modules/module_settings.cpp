#include "module_settings.h"
#include "settings/settings.h"

std::string ModuleSettings::GetString(const char * setting) const
{
    return Settings::GetInstance().GetString(setting);
}

bool ModuleSettings::GetBool(const char * setting) const
{
    return Settings::GetInstance().GetBool(setting);
}

void ModuleSettings::SetString(const char * setting, const char * value)
{
    Settings::GetInstance().SetString(setting, value);
}

void ModuleSettings::SetBool(const char * setting, bool value)
{
    Settings::GetInstance().SetBool(setting, value);
}
