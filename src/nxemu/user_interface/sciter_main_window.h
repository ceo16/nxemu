#pragma once
#include <nxemu-core/modules/module_base.h>
#include <sciter_ui.h>

class SciterMainWindow :
    public IWindowDestroySink,
    public IRenderWindow
{

public:
    SciterMainWindow(ISciterUI & SciterUI);

    bool Show(void);

private:
    SciterMainWindow(void) = delete;
    SciterMainWindow(const SciterMainWindow&) = delete;
    SciterMainWindow& operator=(const SciterMainWindow&) = delete;

    // IWindowDestroySink
    void OnWindowDestroy(HWINDOW hWnd);

    // IRenderWindow
    void * RenderSurface(void) const;

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    void * m_renderWindow;
};

