#include "settings_page_modules.h"
#include <common/std_string.h>
#include <nxemu-core/settings/core_settings.h>
#include <nxemu-core/settings/identifiers.h>

OptionModulesPage::OptionModulesPage(HWND parent, const RECT & dispayRect)
{
    Create(parent);
    if (IsWindow())
    {
        SetWindowHwnd(m_hWnd);
        PositionWindow(dispayRect);
    }

    AddModule(IDC_CPU_LIST, NXCoreSetting::ModuleCpuSelected, MODULE_TYPE_CPU);
    AddModule(IDC_VIDEO_LIST, NXCoreSetting::ModuleVideoSelected, MODULE_TYPE_VIDEO);
    AddModule(IDC_OPERATINGSYSTEM_LIST, NXCoreSetting::ModuleOsSelected, MODULE_TYPE_OPERATING_SYSTEM);
}

const wchar_t * OptionModulesPage::PageTitle(void)
{
    return L"Modules";
}

void OptionModulesPage::AddModule(uint32_t ListId, const char * setttingId, MODULE_TYPE moduleType)
{
    CModifiedComboBoxTxt * comboBox = AddModComboBoxTxt(GetDlgItem(ListId), setttingId);
    for (int i = 0, n = m_modules.GetModuleCount(); i < n; i++)
    {
        const ModuleList::MODULE * module = m_modules.GetModuleInfo(i);
        if (module == nullptr || module->info.type != moduleType)
        {
            continue;
        }
        comboBox->AddItem(stdstr(module->info.name).ToUTF16().c_str(), module->file.c_str());
    }
}