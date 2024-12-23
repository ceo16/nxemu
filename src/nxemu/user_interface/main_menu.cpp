#include "main_menu.h"
#include "main_window.h"
#include "menu_item.h"

MainMenu::MainMenu(MainWindow * Window) :
    m_window(Window),
    m_menu(nullptr)
{
    ResetMenu();
}

MainMenu::~MainMenu()
{
}

HMENU MainMenu::GetHandle(void) const
{
    return m_menu;
}

void MainMenu::ResetMenu(void)
{
    HMENU menu = CreateMenu();
    FillOutMenu(menu);

    HMENU oldMenu = m_menu;
    m_menu = menu;
    m_window->ResetMenu();
    DestroyMenu(oldMenu);
}

void MainMenu::FillOutMenu(HMENU menu)
{
    MenuItemList fileMenu;
    fileMenu.emplace_back(MenuItem(ID_FILE_EXIT, L"E&xit"));

    MenuItemList mainTitleMenu;
    mainTitleMenu.push_back(MenuItem(MenuItem::SUB_MENU, L"&File", &fileMenu));

    AddMenu(menu, mainTitleMenu);
}

bool MainMenu::AddMenu(HMENU menu, const MenuItemList & items)
{
    if (items.begin() == items.end()) { return false; }

    HMENU itemID;
    UINT flags;
    std::wstring text, string;
    for (MenuItemList::const_iterator menuItem = items.begin(); menuItem != items.end(); menuItem++)
    {
        itemID = (HMENU)((UINT_PTR)menuItem->ID());
        flags = MF_STRING | MFS_ENABLED;
        text = menuItem->Title();

        if (itemID == (HMENU)MenuItem::SPLITER)
        {
            flags |= MF_SEPARATOR;
        }

        const MenuItemList * SubMenu = menuItem->SubMenu();
        if (itemID == (HMENU)MenuItem::SUB_MENU && HIWORD(SubMenu) != 0 && (SubMenu->begin() != SubMenu->end()))
        {
            itemID = CreatePopupMenu();
            flags |= MF_POPUP;

            AddMenu(itemID, *SubMenu);
        }

        AppendMenu(menu, flags, (UINT_PTR)itemID, text.c_str());
    }
    return true;
}
