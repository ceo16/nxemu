#pragma once
#include <memory>
#include <string>
#include <vector>

class SettingsPage;

class ConfigSettingSection
{
public:
    ConfigSettingSection(const wchar_t * pageTitle);

    void AddPage(std::shared_ptr<SettingsPage> page);
    SettingsPage * GetPage(uint32_t pageNo);

    const wchar_t * GetPageTitle(void) const;
    size_t GetPageCount(void) const;

private:
    ConfigSettingSection() = delete;
    ConfigSettingSection(const ConfigSettingSection &) = delete;
    ConfigSettingSection & operator=(const ConfigSettingSection &) = delete;

    typedef std::vector<std::shared_ptr<SettingsPage>> SETTING_PAGES;

    SETTING_PAGES m_pages;
    std::wstring m_pageTitle;
};
