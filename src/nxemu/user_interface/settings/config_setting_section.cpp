#include "config_setting_section.h"

ConfigSettingSection::ConfigSettingSection(const wchar_t * pageTitle) :
    m_pageTitle(pageTitle)
{
}

void ConfigSettingSection::AddPage(std::shared_ptr<SettingsPage> page)
{
    m_pages.push_back(page);
}

SettingsPage * ConfigSettingSection::GetPage(uint32_t pageNo)
{
    if (pageNo < 0 || pageNo >= (uint32_t)m_pages.size())
    {
        return NULL;
    }
    return m_pages[pageNo].get();
}

const wchar_t * ConfigSettingSection::GetPageTitle(void) const
{
    return m_pageTitle.c_str();
}

size_t ConfigSettingSection::GetPageCount(void) const
{
    return m_pages.size();
}