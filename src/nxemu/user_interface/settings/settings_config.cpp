#include "settings_config.h"
#include "settings_page_modules.h"
#include <nxemu-core/settings/settings.h>

SettingConfig::SettingConfig() :
    m_currentPage(nullptr),
    m_selChangedSupported(false)
{
}

SettingConfig::~SettingConfig()
{
}

void SettingConfig::Display(HWND parentWindow)
{
    DoModal(parentWindow);
}

LRESULT SettingConfig::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    m_pagesTreeList.Attach(GetDlgItem(IDC_PAGELIST));

    RECT rcSettingInfo;
    GetDlgItem(IDC_SETTING_INFO).GetWindowRect(&rcSettingInfo);
    ::MapWindowPoints(nullptr, m_hWnd, (LPPOINT)&rcSettingInfo, 2);

    std::shared_ptr<ConfigSettingSection> SettingsSection;
    SettingsSection = std::make_shared<ConfigSettingSection>(L"Modules");
    SettingsSection->AddPage(std::make_shared<OptionModulesPage>(this->m_hWnd, rcSettingInfo));
    m_sections.push_back(SettingsSection);

    bool bFirstItem = true;
    for (SettingSections::const_iterator iter = m_sections.begin(); iter != m_sections.end(); iter++)
    {
        ConfigSettingSection * section = iter->get();

        HTREEITEM hSectionItem = nullptr;
        for (uint32_t i = 0, n = (uint32_t)section->GetPageCount(); i < n; i++)
        {
            SettingsPage * page = section->GetPage(i);
            if (hSectionItem == nullptr)
            {
                hSectionItem = m_pagesTreeList.InsertItem(TVIF_TEXT | TVIF_PARAM, section->GetPageTitle(), 0, 0, 0, 0, (LPARAM)page, TVI_ROOT, TVI_LAST);
            }
            else
            {
                m_pagesTreeList.InsertItem(TVIF_TEXT | TVIF_PARAM, page->PageTitle(), 0, 0, 0, 0, (LPARAM)page, hSectionItem, TVI_LAST);
            }
        }
        if (bFirstItem && hSectionItem != nullptr)
        {
            bFirstItem = false;
            m_pagesTreeList.Expand(hSectionItem);
            m_pagesTreeList.SelectItem(hSectionItem);
        }
    }
    return TRUE;
}

LRESULT SettingConfig::OnPageListItemChanged(NMHDR * /*phdr*/)
{
    m_selChangedSupported = true;

    HTREEITEM hItem = m_pagesTreeList.GetSelectedItem();
    SettingsPage * page = (SettingsPage *)m_pagesTreeList.GetItemData(hItem);

    if (page != nullptr)
    {
        if (m_currentPage != nullptr)
        {
            m_currentPage->HidePage();
        }
        m_currentPage = page;
        m_currentPage->ShowPage();
        ::EnableWindow(GetDlgItem(IDC_RESET_PAGE), m_currentPage->EnableReset());
    }
    return 0;
}

LRESULT SettingConfig::OnPageListClicked(NMHDR *)
{
    if (m_selChangedSupported)
    {
        return 0;
    }

    DWORD clickPos = GetMessagePos();
    CPoint clickPt = CPoint(clickPos);
    ScreenToClient(&clickPt);

    CRect treeRect;
    m_pagesTreeList.GetWindowRect(treeRect);
    ScreenToClient(&treeRect);

    clickPt.x -= treeRect.left;
    clickPt.y -= treeRect.top;
    clickPt.y -= 2;

    UINT flags;
    HTREEITEM item = m_pagesTreeList.HitTest(clickPt, &flags);

    SettingsPage * page = (SettingsPage *)m_pagesTreeList.GetItemData(item);

    if (page != nullptr)
    {
        if (m_currentPage != nullptr)
        {
            m_currentPage->HidePage();
        }
        m_currentPage = page;
        m_currentPage->ShowPage();
        ::EnableWindow(GetDlgItem(IDC_RESET_PAGE), m_currentPage->EnableReset());
    }
    return 0;
}

LRESULT SettingConfig::OnSettingPageChanged(UINT /*uMsg*/, WPARAM /*wPage*/, LPARAM /*lParam*/)
{
    GetDlgItem(IDC_APPLY).EnableWindow(true);
    GetDlgItem(IDC_RESET_PAGE).EnableWindow(m_currentPage->EnableReset());
    BoldChangedPages(m_pagesTreeList.GetRootItem());
    return 0;
}

LRESULT SettingConfig::OnApply(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    ApplySettings(true);
    return 0;
}

LRESULT SettingConfig::OnOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    ApplySettings(false);
    EndDialog(1);
    return 0;
}

LRESULT SettingConfig::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    EndDialog(0);
    return 0;
}

LRESULT SettingConfig::OnResetPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    if (m_currentPage)
    {
        m_currentPage->ResetPage();
    }
    return 0;
}

LRESULT SettingConfig::OnResetAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    for (SettingSections::const_iterator iter = m_sections.begin(); iter != m_sections.end(); iter++)
    {
        ConfigSettingSection & section = *(iter->get());

        for (uint32_t i = 0; i < section.GetPageCount(); i++)
        {
            SettingsPage * page = section.GetPage(i);
            if (page->EnableReset())
            {
                page->ResetPage();
            }
        }
    }
    return 0;
}

void SettingConfig::BoldChangedPages(HTREEITEM hItem)
{
    if (hItem == m_pagesTreeList.GetRootItem())
    {
        ::EnableWindow(GetDlgItem(IDC_RESET_ALL), false);
    }
    bool enableResetAll = false;

    while (hItem)
    {
        SettingsPage * page = (SettingsPage *)m_pagesTreeList.GetItemData(hItem);
        if (page)
        {
            m_pagesTreeList.SetItemState(hItem, page->EnableReset() ? TVIS_BOLD : 0, TVIS_BOLD);
            if (page->EnableReset())
            {
                enableResetAll = true;
            }
        }

        BoldChangedPages(m_pagesTreeList.GetChildItem(hItem));
        hItem = m_pagesTreeList.GetNextSiblingItem(hItem);
    }

    if (enableResetAll)
    {
        ::EnableWindow(GetDlgItem(IDC_RESET_ALL), true);
    }
}

void SettingConfig::ApplySettings(bool updateScreen)
{
    for (SettingSections::const_iterator iter = m_sections.begin(); iter != m_sections.end(); iter++)
    {
        ConfigSettingSection * section = iter->get();
        for (uint32_t i = 0; i < section->GetPageCount(); i++)
        {
            SettingsPage * page = section->GetPage(i);
            page->ApplySettings(updateScreen);
        }
    }

    if (updateScreen)
    {
        GetDlgItem(IDC_APPLY).EnableWindow(false);
        GetDlgItem(IDC_RESET_PAGE).EnableWindow(m_currentPage->EnableReset());
    }
    Settings::GetInstance().Save();
}
