#pragma once
#include "user_interface/settings/config_setting_section.h"
#include "user_interface/wtl.h"
#include <list>
#include <memory>

class SettingConfig :
    public CDialogImpl<SettingConfig>
{
public:
    BEGIN_MSG_MAP_EX(SettingConfig)
    {
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog);
        NOTIFY_HANDLER_EX(IDC_PAGELIST, TVN_SELCHANGED, OnPageListItemChanged);
        NOTIFY_HANDLER_EX(IDC_PAGELIST, NM_CLICK, OnPageListClicked);
        MESSAGE_HANDLER_EX(PSM_CHANGED, OnSettingPageChanged);
        COMMAND_ID_HANDLER(IDC_APPLY, OnApply);
        COMMAND_ID_HANDLER(IDOK, OnOk);
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel);
        COMMAND_ID_HANDLER(IDC_RESET_PAGE, OnResetPage);
        COMMAND_ID_HANDLER(IDC_RESET_ALL, OnResetAll);
        REFLECT_NOTIFICATIONS();
    }
    END_MSG_MAP()

    enum
    {
        IDD = IDD_Settings_Config
    };

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnPageListItemChanged(NMHDR * phdr);
    LRESULT OnPageListClicked(NMHDR * phdr);
    LRESULT OnSettingPageChanged(UINT uMsg, WPARAM wPage, LPARAM lParam);
    LRESULT OnApply(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnResetPage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnResetAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    SettingConfig(void);
    ~SettingConfig(void);

    void Display(HWND parentWindow);

private:
    SettingConfig(const SettingConfig &) = delete;
    SettingConfig & operator=(const SettingConfig &) = delete;

    typedef std::list<std::shared_ptr<ConfigSettingSection>> SettingSections;

    void BoldChangedPages(HTREEITEM item);
    void ApplySettings(bool updateScreen);

    CTreeViewCtrl m_pagesTreeList;
    SettingSections m_sections;
    SettingsPage * m_currentPage;
    bool m_selChangedSupported;
};
