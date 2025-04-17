#pragma once
#include "main_menu.h"
#include "wtl.h"
#include <nxemu-core/modules/module_base.h>
#include <string>

class MainWindow :
    public IRenderWindow
{
    enum
    {
        Width = 640,
        Height = 480,
    };

    BEGIN_MSG_MAP(MainWindow)
    {
        MESSAGE_HANDLER(WM_CREATE, OnCreate);
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy);
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown);
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp);
        COMMAND_ID_HANDLER(MainMenu::ID_FILE_OPEN_GAME, OnOpenGame);
        COMMAND_ID_HANDLER(MainMenu::ID_FILE_EXIT, OnFileExit);
        COMMAND_ID_HANDLER(MainMenu::ID_OPTIONS_SETTINGS, OnSettings);
        COMMAND_RANGE_HANDLER(MainMenu::ID_RECENT_FILE_START, MainMenu::ID_RECENT_FILE_END, OnRecetGame);
    }
    END_MSG_MAP()

public:
    MainWindow(const wchar_t * WindowTitle);
    ~MainWindow();

    WPARAM ProcessAllMessages(void) const;
    void ResetMenu();
    void ShowWindow(bool show);

    //IRenderWindow
    void * RenderSurface(void) const;

private:
    MainWindow(void) = delete;
    MainWindow(const MainWindow &) = delete;
    MainWindow & operator=(const MainWindow &) = delete;

    void Create(void);
    bool RegisterWinClass(void);
    std::string ChooseFileToOpen(HWND parent);
    void GameFileChanged(void);
    void GameNameChanged(void);

    static int VKCodeToSwitchKey(uint32_t vkcode);

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnOpenGame(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnRecetGame(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    static LRESULT __stdcall GuiProc(HWND, UINT, WPARAM, LPARAM);

    MainMenu m_menu;
    HWND m_hWnd;
    std::wstring m_className;
    std::wstring m_windowTitle;
};
