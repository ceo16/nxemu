#pragma once
#include <memory>
#include <nxemu-core/modules/module_base.h>
#include <sciter_ui.h>
#include <widgets/menubar.h>

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
        ID_FILE_OPEN_GAME = 1,
        ID_FILE_EXIT,

        // Options Menu
        ID_OPTIONS_INPUT,

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

    void SetCaption(const std::string & caption);
    void GameFileChanged(void);
    void GameNameChanged(void);
    void RomLoadingChanged(void);
    void DisplayedFramesChanged(void);
    void ShowLoadingScreen(void);
    int32_t SciterKeyToSwitchKey(SciterKeys key);
    int32_t SciterKeyToVKCode(SciterKeys vkcode);
        
    void OnOpenGame(void);
    void OnFileExit(void);
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
};
