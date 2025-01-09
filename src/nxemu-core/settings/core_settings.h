#pragma once
#include "common/path.h"
#include <string>

namespace NXCoreSetting
{
constexpr const char * GameFile = "nxcore:GameFile";
constexpr const char * GameName = "nxcore:GameName";
constexpr const char * ModuleCpuSelected = "nxcore:ModuleCpuSelected";
constexpr const char * ModuleVideoSelected = "nxcore:ModuleVideoSelected";
constexpr const char * ModuleOsSelected = "nxcore:ModuleOsSelected";
} // namespace NXCoreSetting

struct CoreSettings
{
    Path configDir;
    Path moduleDir;
    std::string moduleDirValue;
    std::string moduleCpuSelected;
    std::string moduleVideoSelected;
    std::string moduleOsSelected;
};

extern CoreSettings coreSettings;

void LoadCoreSetting(void);
void SaveCoreSetting(void);