#pragma once
#include "user_interface/wtl.h"
#include "user_interface/wtl_controls/modified_combo_box.h"
#include <string>
#include <unordered_map>

class SettingsPage
{
    typedef std::unordered_map<std::string, std::unique_ptr<CModifiedComboBoxTxt>> ComboBoxTxtList;

public:
    SettingsPage(void);
    virtual ~SettingsPage(void) = 0;

    virtual const wchar_t * PageTitle(void) = 0;

    void SetWindowHwnd(HWND hWindow);
    void PositionWindow(const RECT & rcDispay);
    virtual void HidePage(void);
    virtual void ShowPage(void);
    virtual void ApplySettings(bool UpdateScreen);
    virtual bool EnableReset(void);
    virtual void ResetPage(void);
    virtual void UpdatePageSettings(void);

    CModifiedComboBoxTxt * AddModComboBoxTxt(HWND hWnd, const char * setting);

private:
    ComboBoxTxtList m_comboBoxTxtList;
    CWindow m_window;
};
