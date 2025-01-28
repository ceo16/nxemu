#pragma once
#include <common/json.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class Settings
{
    typedef std::unordered_map<std::string, std::string> SettingsMapString;
    typedef std::unordered_map<std::string, bool> SettingsMapBool;
    typedef std::vector<std::function<void()>> NotificationCallbacks;
    typedef std::unordered_map<std::string, NotificationCallbacks> NotificationMap;

public:
    Settings();

    bool Initialize();
    const char * GetConfigFile(void) const;

    JsonValue GetSettings(const char * section) const;
    void SetSettings(const char * section, JsonValue & json);

    std::string GetDefaultString(const char * setting) const;
    bool GetDefaultBool(const char * setting) const;
    std::string GetString(const char * setting) const;
    bool GetBool(const char * setting) const;
    bool GetChanged(const char * setting) const;
    void SetDefaultString(const char * setting, const char * value);
    void SetDefaultBool(const char * setting, bool value);
    void SetString(const char * setting, const char * value);
    void SetBool(const char * setting, bool value);
    void SetChanged(const char * setting, bool changed);

    void Save(void);

    void RegisterCallback(const std::string & setting, std::function<void()> callback);

    static Settings & GetInstance();
    static void CleanUp();

private:
    void NotifyChange(const char * setting);

    SettingsMapString m_settingsString;
    SettingsMapBool m_settingsBool;
    SettingsMapString m_settingsDefaultString;
    SettingsMapBool m_settingsDefaultBool;
    SettingsMapBool m_settingsChanged;
    NotificationMap m_notification;

    static std::unique_ptr<Settings> s_instance;
    std::string m_configPath;
    JsonValue m_details;
};
