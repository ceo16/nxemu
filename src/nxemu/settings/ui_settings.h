#pragma once
#include <string>
#include <vector>

typedef std::vector<std::string> Stringlist;

struct UISettings
{
    Stringlist recentFiles;
    bool enableModuleConfiguration;
};

extern UISettings uiSettings;

void LoadUISetting(void);
void SaveUISetting(void);