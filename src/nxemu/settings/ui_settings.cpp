#include "ui_settings.h"
#include <common/json.h>
#include <common/path.h>
#include <nxemu-core/settings/settings.h>

namespace
{
struct UISettingsDefaults
{
    static constexpr const char * defaultLanguageDirValue = "./lang";
    static constexpr const char * defaultLanguageBaseValue = "english";
    static constexpr const char * defaultLanguageCurrentValue = "english";
    static constexpr bool defaultSciterConsole = false;

    static Path GetDefaultLanguageDir();
};
} // namespace

UISettings uiSettings = {};

void LoadUISetting(void)
{
    uiSettings = {};
    uiSettings.languageDir = UISettingsDefaults::GetDefaultLanguageDir();
    uiSettings.languageDirValue = UISettingsDefaults::defaultLanguageDirValue;
    uiSettings.languageBase = UISettingsDefaults::defaultLanguageBaseValue;
    uiSettings.languageCurrent = UISettingsDefaults::defaultLanguageCurrentValue;
    uiSettings.sciterConsole = UISettingsDefaults::defaultSciterConsole;

    JsonValue jsonSettings = SettingsStore::GetInstance().GetSettings("UI");

    const JsonValue * node = jsonSettings.Find("LanguageDirectory");
    if (node != nullptr && node->isString())
    {
        std::string dirValue = node->asString();
        if (!dirValue.empty())
        {
            uiSettings.languageDirValue = dirValue;
            uiSettings.languageDir = Path(uiSettings.languageDirValue, "").DirectoryNormalize(Path(Path::MODULE_DIRECTORY));
        }
    }

    node = jsonSettings.Find("SciterConsole");
    if (node != nullptr && node->isBool())
    {
        uiSettings.sciterConsole = node->asBool();
    }

    node = jsonSettings.Find("RecentFiles");
    if (node != nullptr && node->isArray())
    {
        for (uint32_t i = 0; i < node->size(); i++)
        {
            const JsonValue & file = (*node)[i];
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

    if (uiSettings.languageDirValue != UISettingsDefaults::defaultLanguageDirValue)
    {
        json["LanguageDirectory"] = JsonValue(uiSettings.languageDirValue);
    }
    SettingsStore & settings = SettingsStore::GetInstance();
    settings.SetSettings("UI", json);
    settings.Save();
}

Path UISettingsDefaults::GetDefaultLanguageDir()
{
    Path dir(Path::MODULE_DIRECTORY);
    dir.AppendDirectory("lang");
    return dir;
}
