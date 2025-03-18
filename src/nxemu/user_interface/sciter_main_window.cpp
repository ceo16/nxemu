#include "sciter_main_window.h"
#include <common/std_string.h>
#include <nxemu-core/version.h>
#include <sciter_element.h>

SciterMainWindow::SciterMainWindow(ISciterUI& SciterUI) :
    m_sciterUI(SciterUI),
    m_renderWindow(nullptr)
{
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
    return true;
}

void SciterMainWindow::OnWindowDestroy(HWINDOW /*hWnd*/)
{
    m_sciterUI.Stop();
}

void * SciterMainWindow::RenderSurface(void) const
{
    return m_renderWindow;
}
