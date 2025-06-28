#pragma once
#include <memory>
#include <nxemu-core/modules/module_base.h>
#include <sciter_ui.h>
#include <sciter_handler.h>
#include <widgets/menubar.h>
#include "user_interface/settings/system_config.h"

class SciterMainWindow :
    public IWindowDestroySink,
    public IMenuBarSink,
    public IRenderWindow,
    public IKeySink,
    public IResizeSink
{
    enum MainMenuID
    {
        // File Menu
        ID_FILE_LOAD_FILE = 1,
        ID_FILE_EXIT,

        // Emulation Menu
        ID_EMULATION_CONTROLLERS,
        ID_EMULATION_CONFIGURE,

        // Recent files
        ID_RECENT_FILE_START,
        ID_RECENT_FILE_END = ID_RECENT_FILE_START + 20,
    };

public:
    SciterMainWindow(ISciterUI & sciterUI, const char * windowTitle);

    void ResetMenu();
    bool Show(void);

private:
    SciterMainWindow(void) = delete;
    SciterMainWindow(const SciterMainWindow &) = delete;
    SciterMainWindow & operator=(const SciterMainWindow &) = delete;

    void SetCaption(const std::string & caption);
    static void GameFileChanged(const char * setting, void * userData);
    static void GameNameChanged(const char * setting, void * userData);
    static void RomLoadingChanged(const char * setting, void * userData);
    static void DisplayedFramesChanged(const char * setting, void * userData);
    void ShowLoadingScreen(void);
    int32_t SciterKeyToSwitchKey(SciterKeys key);
    int32_t SciterKeyToVKCode(SciterKeys vkcode);
        
    void OnOpenFile(void);
    void OnFileExit(void);
    void OnSystemConfig(void);
    void OnInputConfig(void);
    void OnRecetGame(uint32_t fileIndex);

    // IWindowDestroySink
    void OnWindowDestroy(HWINDOW hWnd) override;

    // IMenuBarSink
    void OnMenuItem(int32_t id, SCITER_ELEMENT item) override;

    // IRenderWindow
    void * RenderSurface(void) const override;
    float PixelRatio(void) const override;

    // IKeySink
    bool OnKeyDown(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyUp(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyChar(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;

    // IResizeSink
    bool OnSizeChanged(SCITER_ELEMENT elem);

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    std::shared_ptr<IMenuBar> m_menuBar;
    void * m_renderWindow;
    std::string m_windowTitle;
    SystemConfig m_systemConfig;
};
