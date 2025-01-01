#pragma once
#include <string>
#include <vector>

typedef std::vector<std::string> Stringlist;

struct UISettings
{
    Stringlist recentFiles;
};

extern UISettings uiSettings;

void LoadUISetting(void);
void SaveUISetting(void);