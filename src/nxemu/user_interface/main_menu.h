#pragma once
#include <Windows.h>
#include "menu_item.h"

class MainWindow;

class MainMenu
{
public:
    enum MainMenuID
    {
        //File Menu
        ID_FILE_EXIT,
    };

    MainMenu(MainWindow * Window);
    ~MainMenu();

    HMENU GetHandle(void) const;
    void ResetMenu(void);

private:
    MainMenu(void) = delete;
    MainMenu(const MainMenu&) = delete;
    MainMenu& operator=(const MainMenu&) = delete;

    void FillOutMenu(HMENU menu);
    static bool AddMenu(HMENU menu, const MenuItemList & items);

    MainWindow * m_window;
    HMENU m_menu;
};