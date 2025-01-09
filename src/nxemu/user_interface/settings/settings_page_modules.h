#pragma once
#include "user_interface/settings/settings_page.h"
#include "user_interface/wtl.h"
#include <nxemu-core/modules/module_list.h>

class OptionModulesPage :
    public CDialogImpl<OptionModulesPage>,
    public SettingsPage
{
    BEGIN_MSG_MAP_EX(OptionModulesPage)
    END_MSG_MAP()

    enum
    {
        IDD = IDD_Settings_Modules
    };

public:
    OptionModulesPage(HWND parent, const RECT & dispayRect);

    const wchar_t * PageTitle(void);

private:
    OptionModulesPage() = delete;
    OptionModulesPage(const OptionModulesPage &) = delete;
    OptionModulesPage & operator=(const OptionModulesPage &) = delete;

    void AddModule(uint32_t listId, const char * setttingId, MODULE_TYPE moduleType);

    ModuleList m_modules;
};
