#include "main_window.h"
#include "settings/settings_config.h"
#include "settings/ui_settings.h"
#include <common/path.h>
#include <common/std_string.h>
#include <nxemu-core/settings/core_settings.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/switch_rom.h>

MainWindow::MainWindow(const wchar_t * windowTitle) :
    m_hWnd(nullptr),
    m_className(L"NxEmu"),
    m_menu(this),
    m_windowTitle(windowTitle)
{
    Settings::GetInstance().RegisterCallback(NXCoreSetting::GameFile, std::bind(&MainWindow::GameFileChanged, this));
    Settings::GetInstance().RegisterCallback(NXCoreSetting::GameName, std::bind(&MainWindow::GameNameChanged, this));

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
    wcl.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_NXEMU_ICO));
    wcl.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcl.hInstance = GetModuleHandle(nullptr);

    wcl.lpfnWndProc = (WNDPROC)GuiProc;
    wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcl.lpszMenuName = nullptr;
    wcl.lpszClassName = m_className.c_str();
    return RegisterClass(&wcl) != 0;
}

std::string MainWindow::ChooseFileToOpen(HWND hParent)
{
    Path fileToOpen;
    const char * filter = "Switch Files (*.nro)\0*.nro\0All files (*.*)\0*.*\0";
    if (fileToOpen.FileSelect(hParent, Path(Path::MODULE_DIRECTORY), filter, true))
    {
        return fileToOpen;
    }
    return "";
}

void MainWindow::GameFileChanged(void)
{
    enum
    {
        maxRememberedFiles = 10
    };

    Stringlist & recentFiles = uiSettings.recentFiles;
    std::string gameFile = Settings::GetInstance().GetString(NXCoreSetting::GameFile);
    for (Stringlist::const_iterator itr = recentFiles.begin(); itr != recentFiles.end(); itr++)
    {
        if (_stricmp(gameFile.c_str(), itr->c_str()) != 0)
        {
            continue;
        }
        recentFiles.erase(itr);
        break;
    }
    recentFiles.insert(recentFiles.begin(), gameFile);
    if (recentFiles.size() > maxRememberedFiles)
    {
        recentFiles.resize(maxRememberedFiles);
    }
    m_menu.ResetMenu();
    SaveUISetting();
}

void MainWindow::Create(void)
{
    CreateWindowExW(WS_EX_ACCEPTFILES, m_className.c_str(), m_windowTitle.c_str(), WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, Width, Height,
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

LRESULT MainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    ResetMenu();

    int x = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;

    SetWindowPos(m_hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    return 0;
}

LRESULT MainWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    PostQuitMessage(0);
    return 0;
}

LRESULT MainWindow::OnOpenGame(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    std::string fileName = ChooseFileToOpen(m_hWnd);
    if (fileName.length() == 0)
    {
        return 0;
    }
    LaunchSwitchRom(fileName.c_str());
    return 0;
}

LRESULT MainWindow::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    DestroyWindow(m_hWnd);
    return 0;
}

LRESULT MainWindow::OnSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    SettingConfig().Display(m_hWnd);
    return 0;
}

LRESULT MainWindow::OnRecetGame(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    uint32_t fileIndex = LOWORD(wID) - MainMenu::ID_RECENT_FILE_START;
    Stringlist & recentFiles = uiSettings.recentFiles;
    if (fileIndex < recentFiles.size())
    {
        LaunchSwitchRom(recentFiles[fileIndex].c_str());
    }
    return 0;
}

LRESULT MainWindow::GuiProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (uintptr_t)lpcs->lpCreateParams);
        MainWindow * _this = (MainWindow *)lpcs->lpCreateParams;
        _this->m_hWnd = hWnd;
    }
    MainWindow * _this = (MainWindow *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (_this)
    {
        LRESULT result = 0;
        if (_this->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, result))
        {
            return result;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void MainWindow::ResetMenu()
{
    SetMenu(m_hWnd, m_menu.GetHandle());
}

void MainWindow::GameNameChanged(void)
{
    std::string gameName = Settings::GetInstance().GetString("nxcore:GameName");
    if (gameName.length() > 0)
    {
        std::wstring Caption;
        Caption += stdstr(gameName).ToUTF16();
        Caption += L" | ";
        Caption += m_windowTitle;
        SetWindowTextW(m_hWnd, Caption.c_str());
    }
}
