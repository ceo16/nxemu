#pragma once
#include "wtl.h"
#include <string>

class MainWindow
{
    enum
    {
        Width = 640,
        Height = 480,
    };

    BEGIN_MSG_MAP(MainWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

public:
    MainWindow(const wchar_t * WindowTitle);
    ~MainWindow();

    WPARAM ProcessAllMessages(void) const;
    void ShowWindow(bool show);

private:
    MainWindow(void) = delete;
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;
    
    void Create(void);
    bool RegisterWinClass(void);

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    static LRESULT __stdcall GuiProc(HWND, UINT, WPARAM, LPARAM);

    HWND m_hWnd;
    std::wstring m_className;
    std::wstring m_windowTitle;
};
