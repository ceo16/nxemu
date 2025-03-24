#include "main_window.h"
#include "settings/settings_config.h"
#include "settings/ui_settings.h"
#include <common/path.h>
#include <common/std_string.h>
#include <nxemu-core/machine/switch_system.h>
#include <nxemu-core/settings/core_settings.h>
#include <nxemu-core/settings/identifiers.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/switch_rom.h>
#include <yuzu_common/settings_input.h>

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

LRESULT MainWindow::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IOperatingSystem & OperatingSystem = system->OperatingSystem();
        int keyIndex = VKCodeToSwitchKey((uint32_t)wParam);
        if (keyIndex != 0)
        {
            OperatingSystem.KeyboardKeyPress(0, keyIndex, wParam);
        }
    }
    return 0;
}

LRESULT MainWindow::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system != nullptr)
    {
        IOperatingSystem & OperatingSystem = system->OperatingSystem();
        int keyIndex = VKCodeToSwitchKey(wParam);
        if (keyIndex != 0)
        {
            OperatingSystem.KeyboardKeyRelease(0, keyIndex, wParam);
        }
    }
    return 0;
}

LRESULT MainWindow::OnOpenGame(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    std::string fileName = ChooseFileToOpen(m_hWnd);
    if (fileName.length() == 0)
    {
        return 0;
    }
    LaunchSwitchRom(*this, fileName.c_str());
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
        LaunchSwitchRom(*this, recentFiles[fileIndex].c_str());
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

void * MainWindow::RenderSurface(void) const
{
    return m_hWnd;
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

int MainWindow::VKCodeToSwitchKey(uint32_t vkcode)
{
    static const std::unordered_map<uint32_t, int> keyMappings = {
        {'A', InputSettings::NativeKeyboard::A},
        {'B', InputSettings::NativeKeyboard::B},
        {'C', InputSettings::NativeKeyboard::C},
        {'D', InputSettings::NativeKeyboard::D},
        {'E', InputSettings::NativeKeyboard::E},
        {'F', InputSettings::NativeKeyboard::F},
        {'G', InputSettings::NativeKeyboard::G},
        {'H', InputSettings::NativeKeyboard::H},
        {'I', InputSettings::NativeKeyboard::I},
        {'J', InputSettings::NativeKeyboard::J},
        {'K', InputSettings::NativeKeyboard::K},
        {'L', InputSettings::NativeKeyboard::L},
        {'M', InputSettings::NativeKeyboard::M},
        {'N', InputSettings::NativeKeyboard::N},
        {'O', InputSettings::NativeKeyboard::O},
        {'P', InputSettings::NativeKeyboard::P},
        {'Q', InputSettings::NativeKeyboard::Q},
        {'R', InputSettings::NativeKeyboard::R},
        {'S', InputSettings::NativeKeyboard::S},
        {'T', InputSettings::NativeKeyboard::T},
        {'U', InputSettings::NativeKeyboard::U},
        {'v', InputSettings::NativeKeyboard::V},
        {'W', InputSettings::NativeKeyboard::W},
        {'X', InputSettings::NativeKeyboard::X},
        {'Y', InputSettings::NativeKeyboard::Y},
        {'Z', InputSettings::NativeKeyboard::Z},
        {'1', InputSettings::NativeKeyboard::N1},
        {'2', InputSettings::NativeKeyboard::N2},
        {'3', InputSettings::NativeKeyboard::N3},
        {'4', InputSettings::NativeKeyboard::N4},
        {'5', InputSettings::NativeKeyboard::N5},
        {'6', InputSettings::NativeKeyboard::N6},
        {'7', InputSettings::NativeKeyboard::N7},
        {'8', InputSettings::NativeKeyboard::N8},
        {'9', InputSettings::NativeKeyboard::N9},
        {'0', InputSettings::NativeKeyboard::N0},
        {VK_RETURN, InputSettings::NativeKeyboard::Return},
        {VK_ESCAPE, InputSettings::NativeKeyboard::Escape},
        {VK_TAB, InputSettings::NativeKeyboard::Tab},
        {VK_SPACE, InputSettings::NativeKeyboard::Space},
        {VK_F1, InputSettings::NativeKeyboard::F1},
        {VK_F2, InputSettings::NativeKeyboard::F2},
        {VK_F3, InputSettings::NativeKeyboard::F3},
        {VK_F4, InputSettings::NativeKeyboard::F4},
        {VK_F5, InputSettings::NativeKeyboard::F5},
        {VK_F6, InputSettings::NativeKeyboard::F6},
        {VK_F7, InputSettings::NativeKeyboard::F7},
        {VK_F8, InputSettings::NativeKeyboard::F8},
        {VK_F9, InputSettings::NativeKeyboard::F9},
        {VK_F10, InputSettings::NativeKeyboard::F10},
        {VK_F11, InputSettings::NativeKeyboard::F11},
        {VK_F12, InputSettings::NativeKeyboard::F12},
        {VK_RIGHT, InputSettings::NativeKeyboard::Right},
        {VK_LEFT, InputSettings::NativeKeyboard::Left},
        {VK_DOWN, InputSettings::NativeKeyboard::Down},
        {VK_UP, InputSettings::NativeKeyboard::Up},
        {VK_F13, InputSettings::NativeKeyboard::F13},
        {VK_F14, InputSettings::NativeKeyboard::F14},
        {VK_F15, InputSettings::NativeKeyboard::F15},
        {VK_F16, InputSettings::NativeKeyboard::F16},
        {VK_F17, InputSettings::NativeKeyboard::F17},
        {VK_F18, InputSettings::NativeKeyboard::F18},
        {VK_F19, InputSettings::NativeKeyboard::F19},
        {VK_F20, InputSettings::NativeKeyboard::F20},
        {VK_F21, InputSettings::NativeKeyboard::F21},
        {VK_F22, InputSettings::NativeKeyboard::F22},
        {VK_F23, InputSettings::NativeKeyboard::F23},
        {VK_F24, InputSettings::NativeKeyboard::F24},
    };

    if (keyMappings.find(vkcode) != keyMappings.end())
    {
        return keyMappings.at(vkcode);
    }
    return 0;
}
