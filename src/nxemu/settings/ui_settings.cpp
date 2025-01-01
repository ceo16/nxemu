#include "ui_settings.h"
#include <common/json.h>
#include <nxemu-core/settings/settings.h>

UISettings uiSettings = {};

void LoadUISetting(void)
{
    uiSettings.recentFiles.clear();

    JsonValue settings = Settings::GetInstance().GetSettings("UI");

    const JsonValue * recentFiles = settings.Find("RecentFiles");
    if (recentFiles != nullptr && recentFiles->isArray())
    {
        for (uint32_t i = 0; i < recentFiles->size(); i++)
        {
            const JsonValue & file = (*recentFiles)[i];
            if (file.isString())
            {
                uiSettings.recentFiles.push_back(file.asString());
            }
        }
    }
}

void SaveUISetting(void)
{
    JsonValue json;
    JsonValue recentFiles(JsonValueType::Array);
    for (const std::string & file : uiSettings.recentFiles)
    {
        recentFiles.Append(JsonValue(file));
    }
    json["RecentFiles"] = std::move(recentFiles);

    Settings & settings = Settings::GetInstance();
    settings.SetSettings("UI", json);
    settings.Save();
}