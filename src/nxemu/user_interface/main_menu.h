#pragma once
#include "menu_item.h"
#include <Windows.h>

class MainWindow;

class MainMenu
{
public:
    enum MainMenuID
    {
        //File Menu
        ID_FILE_OPEN_GAME,
        ID_FILE_EXIT,

        //Options Menu
        ID_OPTIONS_SETTINGS,

        // Recent files
        ID_RECENT_FILE_START,
        ID_RECENT_FILE_END = ID_RECENT_FILE_START + 20,
    };

    MainMenu(MainWindow * Window);
    ~MainMenu();

    HMENU GetHandle(void) const;
    void ResetMenu(void);

private:
    MainMenu(void) = delete;
    MainMenu(const MainMenu &) = delete;
    MainMenu & operator=(const MainMenu &) = delete;

    void FillOutMenu(HMENU menu);
    static bool AddMenu(HMENU menu, const MenuItemList & items);

    MainWindow * m_window;
    HMENU m_menu;
};