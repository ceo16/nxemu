#include "menu_item.h"

MenuItem::MenuItem(int32_t id, wchar_t * title, MenuItemList * subMenu)
{
    Reset(id, title, subMenu);
}

void MenuItem::Reset(int32_t id, wchar_t * title, MenuItemList * subMenu)
{
    m_id = id;
    m_title = title;
    m_subMenu = subMenu;
}

int MenuItem::ID() const
{
    return m_id;
}

const wchar_t* MenuItem::Title() const
{
    return m_title.c_str();
}

const MenuItemList * MenuItem::SubMenu() const
{
    return m_subMenu;
}

