#include "ui_settings.h"
#include <common/json.h>
#include <nxemu-core/settings/settings.h>

namespace
{
struct UISettingsDefaults
{
    static constexpr bool enableModuleConfiguration = false;
};
} // namespace

UISettings uiSettings = {};

void LoadUISetting(void)
{
    uiSettings = {};
    uiSettings.enableModuleConfiguration = UISettingsDefaults::enableModuleConfiguration;

    JsonValue settings = Settings::GetInstance().GetSettings("UI");

    const JsonValue * modConfig = settings.Find("EnableModuleConfiguration");
    if (modConfig != nullptr && modConfig->isBool())
    {
        uiSettings.enableModuleConfiguration = modConfig->asBool();
    }

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
    if (!uiSettings.recentFiles.empty())
    {
        JsonValue recentFiles(JsonValueType::Array);
        for (const std::string & file : uiSettings.recentFiles)
        {
            recentFiles.Append(JsonValue(file));
        }
        json["RecentFiles"] = std::move(recentFiles);
    }

    if (uiSettings.enableModuleConfiguration != UISettingsDefaults::enableModuleConfiguration)
    {
        json["EnableModuleConfiguration"] = JsonValue(uiSettings.enableModuleConfiguration);
    }

    Settings & settings = Settings::GetInstance();
    settings.SetSettings("UI", json);
    settings.Save();
}