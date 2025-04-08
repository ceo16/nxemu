#pragma once
#include <memory>
#include <nxemu-core/modules/module_base.h>
#include <sciter_ui.h>
#include <widgets/menubar.h>

class SciterMainWindow :
    public IWindowDestroySink,
    public IMenuBarSink,
    public IRenderWindow
{
    enum MainMenuID
    {
        // File Menu
        ID_FILE_OPEN_GAME = 1,
        ID_FILE_EXIT,

        // Options Menu
        ID_OPTIONS_SETTINGS,

        // Recent files
        ID_RECENT_FILE_START,
        ID_RECENT_FILE_END = ID_RECENT_FILE_START + 20,
    };

public:
    SciterMainWindow(ISciterUI & SciterUI, const char * windowTitle);

    void ResetMenu();
    bool Show(void);

private:
    SciterMainWindow(void) = delete;
    SciterMainWindow(const SciterMainWindow &) = delete;
    SciterMainWindow & operator=(const SciterMainWindow &) = delete;

    void SetCaption(const std::string& caption);
    void GameFileChanged(void);
    void GameNameChanged(void);
    void RomLoadingChanged(void);
    void DisplayedFramesChanged(void);
    void ShowLoadingScreen(void);

    void OnOpenGame(void);
    void OnFileExit(void);
    void OnSettings(void);
    void OnRecetGame(uint32_t fileIndex);

    // IWindowDestroySink
    void OnWindowDestroy(HWINDOW hWnd);

    // IMenuBarSink
    void OnMenuItem(int32_t id, SCITER_ELEMENT item);

    // IRenderWindow
    void * RenderSurface(void) const;

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    std::shared_ptr<IMenuBar> m_menuBar;
    void * m_renderWindow;
    std::string m_windowTitle;
};
