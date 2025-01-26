#include "settings_page.h"
#include <nxemu-core/settings/settings.h>

SettingsPage::SettingsPage(void)
{
}

SettingsPage::~SettingsPage(void)
{
}

void SettingsPage::SetWindowHwnd(HWND hWindow)
{
    m_window.m_hWnd = hWindow;
}

void SettingsPage::PositionWindow(const RECT & rcDispay)
{
    if (m_window.IsWindow())
    {
        m_window.SetWindowPos(HWND_TOP, &rcDispay, SWP_HIDEWINDOW);
    }
}

void SettingsPage::HidePage(void)
{
    if (m_window.IsWindow())
    {
        m_window.ShowWindow(SW_HIDE);
    }
}

void SettingsPage::ShowPage(void)
{
    if (m_window.IsWindow())
    {
        m_window.ShowWindow(SW_SHOW);
    }
}

void SettingsPage::ApplySettings(bool UpdateScreen)
{
    Settings & settings = Settings::GetInstance();

    for (ComboBoxTxtList::iterator itr = m_comboBoxTxtList.begin(); itr != m_comboBoxTxtList.end(); itr++)
    {
        std::string type = itr->first;
        CModifiedComboBoxTxt & comboBox = *(itr->second);
        if (comboBox.IsChanged())
        {
            int index = comboBox.GetCurSel();
            if (index == CB_ERR)
            {
                continue;
            }
            std::string * ItemData = (std::string *)comboBox.GetItemData(index);
            settings.SetString(type.c_str(), ItemData->c_str());
        }
        if (comboBox.IsReset())
        {
            settings.SetString(type.c_str(), comboBox.DefaultValue().c_str());
            comboBox.SetReset(false);
        }
    }

    if (UpdateScreen)
    {
        UpdatePageSettings();
    }
}

bool SettingsPage::EnableReset(void)
{
    for (ComboBoxTxtList::iterator itr = m_comboBoxTxtList.begin(); itr != m_comboBoxTxtList.end(); itr++)
    {
        CModifiedComboBoxTxt & comboBox = *(itr->second);
        if (comboBox.IsChanged())
        {
            return true;
        }
    }
    return false;
}

void SettingsPage::ResetPage(void)
{
    Settings & settings = Settings::GetInstance();
    bool Changed = false;
    for (ComboBoxTxtList::iterator itr = m_comboBoxTxtList.begin(); itr != m_comboBoxTxtList.end(); itr++)
    {
        CModifiedComboBoxTxt & comboBox = *(itr->second);
        if (!comboBox.IsChanged())
        {
            continue;
        }

        comboBox.SetReset(true);
        std::string value = settings.GetDefaultString(itr->first.c_str());
        for (int i = 0, n = comboBox.GetCount(); i < n; i++)
        {
            const std::string * itemData = (std::string *)comboBox.GetItemData(i);
            if (strcmp(itemData->c_str(), value.c_str()) != 0)
            {
                continue;
            }
            comboBox.SetCurSel(i);
            Changed = true;
            break;
        }
    }
    if (Changed)
    {
        SendMessage(m_window.GetParent(), PSM_CHANGED, (WPARAM)m_window.m_hWnd, 0);
    }
}

void SettingsPage::UpdatePageSettings(void)
{
    Settings & settings = Settings::GetInstance();
    for (ComboBoxTxtList::iterator itr = m_comboBoxTxtList.begin(); itr != m_comboBoxTxtList.end(); itr++)
    {
        CModifiedComboBoxTxt & comboBox = *(itr->second);
        std::string selectedValue = settings.GetString(itr->first.c_str());

        comboBox.SetChanged(settings.GetChanged(itr->first.c_str()));
        comboBox.SetSelected(selectedValue);
    }
}

CModifiedComboBoxTxt * SettingsPage::AddModComboBoxTxt(HWND hWnd, const char * setting)
{
    ComboBoxTxtList::const_iterator itr = m_comboBoxTxtList.find(setting);
    if (itr != m_comboBoxTxtList.end())
    {
        return itr->second.get();
    }

    Settings & settings = Settings::GetInstance();
    std::string defaultValue = settings.GetDefaultString(setting);
    std::string selectedValue = settings.GetString(setting);

    std::unique_ptr<CModifiedComboBoxTxt> comboBox = std::make_unique<CModifiedComboBoxTxt>(selectedValue, defaultValue, hWnd, false);
    if (comboBox.get() != nullptr)
    {
        CModifiedComboBoxTxt * comboBoxPtr = comboBox.get();
        m_comboBoxTxtList[setting] = std::move(comboBox);
        return comboBoxPtr;
    }
    return nullptr;
}
