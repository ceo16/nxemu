#pragma once
#include <list>
#include <stdint.h>
#include <string>

class MenuItem;
typedef std::list<MenuItem> MenuItemList;

class MenuItem
{
public:
    enum MenuID
    {
        SPLITER = -1,
        SUB_MENU = -2,
        NO_ID = -3
    };

    MenuItem(int32_t id, const wchar_t * title = L"", MenuItemList * subMenu = nullptr);
    void Reset(int32_t id, const wchar_t * title = L"", MenuItemList * subMenu = nullptr);

    int ID() const;
    const wchar_t * Title() const;
    const MenuItemList * SubMenu() const;

private:
    MenuItem(void) = delete;
    MenuItem & operator=(const MenuItem &) = delete;

    int32_t m_id;
    std::wstring m_title;
    MenuItemList * m_subMenu;
};