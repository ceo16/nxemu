#include "sciter_main_window.h"
#include "settings/ui_settings.h"
#include <common/std_string.h>
#include <nxemu-core/switch_rom.h>
#include <nxemu-core/version.h>
#include <sciter_element.h>
#include <widgets/menubar.h>

SciterMainWindow::SciterMainWindow(ISciterUI & SciterUI) :
    m_sciterUI(SciterUI),
    m_window(nullptr),
    m_renderWindow(nullptr)
{
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

    if (!m_sciterUI.WindowCreate(nullptr, "main_window.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, m_window))
    {
        return false;
    }

    m_window->OnDestroySinkAdd(this);
    m_window->CenterWindow();
    stdstr_f windowTitle("NxEmu - %s", VER_FILE_VERSION_STR);
    SciterElement rootElement(m_window->GetRootElement());
    rootElement.Eval(stdstr_f("Window.this.caption = \"%s\";", windowTitle.c_str()).c_str());
    SciterElement caption(rootElement.FindFirst("[role='window-caption'] > span"));
    if (caption.IsValid())
    {
        caption.SetHTML((uint8_t *)windowTitle.data(), windowTitle.size());
    }
    SciterElement menuElement(rootElement.GetElementByID("MainMenu"));
    void * interfacePtr = nullptr;
    if (menuElement.IsValid() && m_sciterUI.GetElementInterface(menuElement, IID_IMENUBAR, &interfacePtr))
    {
        m_menuBar = std::shared_ptr<IMenuBar>(reinterpret_cast<IMenuBar *>(interfacePtr), [](IMenuBar *) {});
        ResetMenu();
    }
    return true;
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
}

void SciterMainWindow::OnRecetGame(uint32_t fileIndex)
{
}

void SciterMainWindow::OnWindowDestroy(HWINDOW /*hWnd*/)
{
    m_sciterUI.Stop();
}

void SciterMainWindow::OnMenuItem(int32_t id, SCITER_ELEMENT item)
{
    switch (id)
    {
    case ID_FILE_OPEN_GAME: OnOpenGame(); break;
    case ID_FILE_EXIT: OnFileExit(); break;
    case ID_OPTIONS_SETTINGS: OnSettings(); break;
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
