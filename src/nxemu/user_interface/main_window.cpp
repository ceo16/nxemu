#include "main_window.h"

MainWindow::MainWindow(const wchar_t * windowTitle) :
    m_hWnd(nullptr),
    m_className(L"NxEmu"),
    m_windowTitle(windowTitle)
{
    RegisterWinClass();
    Create();
}

MainWindow::~MainWindow()
{
    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
    }
}

bool MainWindow::RegisterWinClass(void)
{
    WNDCLASS wcl;

    wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcl.cbClsExtra = 0;
    wcl.cbWndExtra = 0;
    wcl.hIcon = nullptr;
    wcl.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcl.hInstance = GetModuleHandle(nullptr);

    wcl.lpfnWndProc = (WNDPROC)GuiProc;
    wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcl.lpszMenuName = nullptr;
    wcl.lpszClassName = m_className.c_str();
    return RegisterClass(&wcl) != 0;
}

void MainWindow::Create(void)
{
    CreateWindowExW(WS_EX_ACCEPTFILES, m_className.c_str(), m_windowTitle.c_str(), WS_OVERLAPPED | WS_CLIPCHILDREN |
        WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, Width, Height,
        nullptr, nullptr, GetModuleHandle(nullptr), this);
}

WPARAM MainWindow::ProcessAllMessages(void) const
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void MainWindow::ShowWindow(bool show)
{
    if (m_hWnd)
    {
        ::ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
    }
}

LRESULT MainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    int x = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
    int	y = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;

    SetWindowPos(m_hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    return 0;
}

LRESULT MainWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    PostQuitMessage(0);
    return 0;
}

LRESULT MainWindow::GuiProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (uintptr_t)lpcs->lpCreateParams);
        MainWindow * _this = (MainWindow*)lpcs->lpCreateParams;
        _this->m_hWnd = hWnd;
    }
    MainWindow * _this = (MainWindow *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (_this)
    {
        LRESULT result;
        if (_this->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, result))
        {
            return result;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

