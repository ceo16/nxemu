#pragma once
#include "common/path.h"
#include <string>

struct CoreSettings
{
    bool showConsole;
    Path configDir;
    Path moduleDir;
    std::string moduleDirValue;
    std::string moduleLoaderSelected;
    std::string moduleCpuSelected;
    std::string moduleVideoSelected;
    std::string moduleOsSelected;
};

extern CoreSettings coreSettings;

void LoadCoreSetting(void);
void SaveCoreSetting(void);