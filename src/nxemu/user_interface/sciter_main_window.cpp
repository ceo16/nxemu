#include "sciter_main_window.h"
#include "settings/input_config.h"
#include "settings/settings_config.h"
#include "settings/ui_settings.h"
#include <Windows.h>
#include <common/std_string.h>
#include <nxemu-core/settings/identifiers.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/switch_rom.h>
#include <nxemu-core/version.h>
#include <sciter_element.h>
#include <widgets/menubar.h>

SciterMainWindow::SciterMainWindow(ISciterUI & SciterUI, const char * windowTitle) :
    m_sciterUI(SciterUI),
    m_window(nullptr),
    m_renderWindow(nullptr),
    m_windowTitle(windowTitle)
{
    Settings & settings = Settings::GetInstance();
    settings.RegisterCallback(NXCoreSetting::GameFile, std::bind(&SciterMainWindow::GameFileChanged, this));
    settings.RegisterCallback(NXCoreSetting::GameName, std::bind(&SciterMainWindow::GameNameChanged, this));
    settings.RegisterCallback(NXCoreSetting::RomLoading, std::bind(&SciterMainWindow::RomLoadingChanged, this));
    settings.RegisterCallback(NXCoreSetting::DisplayedFrames, std::bind(&SciterMainWindow::DisplayedFramesChanged, this));
}

void SciterMainWindow::ResetMenu()
{
    if (m_menuBar == nullptr)
    {
        return;
    }
    MenuBarItemList fileMenu;
    fileMenu.push_back(MenuBarItem(ID_FILE_OPEN_GAME, "Open Game..."));

    Stringlist & recentFiles = uiSettings.recentFiles;
    MenuBarItemList RecentGameMenu;
    if (recentFiles.size() > 0)
    {
        int32_t recentFileIndex = 0;
        for (Stringlist::const_iterator itr = recentFiles.begin(); itr != recentFiles.end(); itr++)
        {
            stdstr_f MenuString("%d %s", recentFileIndex + 1, itr->c_str());
            RecentGameMenu.push_back(MenuBarItem(ID_RECENT_FILE_START + recentFileIndex, MenuString.c_str()));
            recentFileIndex += 1;
        }
        fileMenu.emplace_back(MenuBarItem::SUB_MENU, "Recent Games", &RecentGameMenu);
    }
    fileMenu.push_back(MenuBarItem(MenuBarItem::SPLITER));
    fileMenu.push_back(MenuBarItem(ID_FILE_EXIT, "Exit"));

    MenuBarItemList OptionsMenu;
    OptionsMenu.push_back(MenuBarItem(ID_OPTIONS_INPUT, "Configure Input..."));
    OptionsMenu.push_back(MenuBarItem(ID_OPTIONS_SETTINGS, "Settings..."));

    MenuBarItemList mainTitleMenu;
    mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "File", &fileMenu));
    if (uiSettings.enableModuleConfiguration)
    {
        mainTitleMenu.push_back(MenuBarItem(MenuBarItem::SUB_MENU, "Options", &OptionsMenu));
    }
    m_menuBar->AddSink(this);
    m_menuBar->SetMenuContent(mainTitleMenu);
}

bool SciterMainWindow::Show(void)
{
    enum
    {
        WINDOW_HEIGHT = 507,
        WINDOW_WIDTH = 760,
    };

    if (!m_sciterUI.WindowCreate(nullptr, "main_window.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SUIW_MAIN, m_window))
    {
        return false;
    }
    SciterElement rootElement(m_window->GetRootElement());
    m_window->OnDestroySinkAdd(this);
    m_window->CenterWindow();
    SetCaption(m_windowTitle);
    SciterElement menuElement(rootElement.GetElementByID("MainMenu"));
    void * interfacePtr = nullptr;
    if (menuElement.IsValid() && m_sciterUI.GetElementInterface(menuElement, IID_IMENUBAR, &interfacePtr))
    {
        m_menuBar = std::shared_ptr<IMenuBar>(reinterpret_cast<IMenuBar *>(interfacePtr), [](IMenuBar *) {});
        ResetMenu();
    }
    m_sciterUI.UpdateWindow(rootElement.GetElementHwnd(true));
    SciterElement::RECT rect = {20, 20, 660, 500};
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        rect = mainContents.GetLocation();
    }

    m_renderWindow = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                                     rect.left, rect.top, rect.right, rect.bottom, (HWND)m_window->GetHandle(), nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow((HWND)m_renderWindow, SW_HIDE);

    return true;
}

void SciterMainWindow::ShowLoadingScreen()
{
    SciterElement rootElement(m_window->GetRootElement());
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        m_sciterUI.SetElementHtmlFromResource(mainContents, "loading.html");
    }
}

void SciterMainWindow::SetCaption(const std::string & caption)
{
    SciterElement rootElement(m_window->GetRootElement());
    rootElement.Eval(stdstr_f("Window.this.caption = \"%s\";", caption.c_str()).c_str());
    SciterElement captionElement(rootElement.FindFirst("[role='window-caption'] > span"));
    if (captionElement.IsValid())
    {
        captionElement.SetHTML((uint8_t *)caption.data(), caption.size());
    }
}

void SciterMainWindow::GameFileChanged(void)
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
    ResetMenu();
    SaveUISetting();
}

void SciterMainWindow::GameNameChanged(void)
{
    std::string gameName = Settings::GetInstance().GetString(NXCoreSetting::GameName);
    if (gameName.length() > 0)
    {
        std::string caption;
        caption += gameName;
        caption += " | ";
        caption += m_windowTitle;
        SetCaption(caption);
    }
}

void SciterMainWindow::RomLoadingChanged(void)
{
    bool loading = Settings::GetInstance().GetBool(NXCoreSetting::RomLoading);
    if (loading)
    {
        ShowLoadingScreen();
    }
}

void SciterMainWindow::DisplayedFramesChanged(void)
{
    SciterElement rootElement(m_window->GetRootElement());
    SciterElement mainContents(rootElement.GetElementByID("MainContents"));
    if (mainContents.IsValid())
    {
        mainContents.SetHTML((uint8_t *)"", 0, SciterElement::SIH_REPLACE_CONTENT);
    }
    ShowWindow((HWND)m_renderWindow, SW_SHOW);
}

void SciterMainWindow::OnOpenGame(void)
{
    Path fileToOpen;
    const char * filter = "Switch Files (*.nro)\0*.nro\0All files (*.*)\0*.*\0";
    if (fileToOpen.FileSelect((void *)m_window->GetHandle(), Path(Path::MODULE_DIRECTORY), filter, true))
    {
        LaunchSwitchRom(*this, fileToOpen);
    }
}

void SciterMainWindow::OnFileExit(void)
{
    m_sciterUI.Stop();
}

void SciterMainWindow::OnSettings(void)
{
    SettingConfig().Display((HWND)m_window->GetHandle());
}

void SciterMainWindow::OnInputConfig(void)
{
    InputConfig(m_sciterUI).Display((void *)m_window->GetHandle());
}

void SciterMainWindow::OnRecetGame(uint32_t fileIndex)
{
    Stringlist & recentFiles = uiSettings.recentFiles;
    if (fileIndex < recentFiles.size())
    {
        LaunchSwitchRom(*this, recentFiles[fileIndex].c_str());
    }
}

void SciterMainWindow::OnWindowDestroy(HWINDOW /*hWnd*/)
{
    m_sciterUI.Stop();
}

void SciterMainWindow::OnMenuItem(int32_t id, SCITER_ELEMENT /*item*/)
{
    switch (id)
    {
    case ID_FILE_OPEN_GAME: OnOpenGame(); break;
    case ID_FILE_EXIT: OnFileExit(); break;
    case ID_OPTIONS_SETTINGS: OnSettings(); break;
    case ID_OPTIONS_INPUT: OnInputConfig(); break;
    default:
        if (id >= ID_RECENT_FILE_START && id <= ID_RECENT_FILE_END)
        {
            OnRecetGame(id - ID_RECENT_FILE_START);
        }
    }
}

void * SciterMainWindow::RenderSurface(void) const
{
    return m_renderWindow;
}
