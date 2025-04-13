#include "input_config.h"
#include <sciter_ui.h>

InputConfig::InputConfig(ISciterUI & SciterUI) :
    m_sciterUI(SciterUI),
    m_window(nullptr)
{
}

InputConfig::~InputConfig()
{
}

void InputConfig::Display(void * parentWindow)
{
    enum
    {
        WINDOW_HEIGHT = 300,
        WINDOW_WIDTH = 300,
    };

    if (!m_sciterUI.WindowCreate(parentWindow, "input_config.html", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SUIW_CHILD, m_window))
    {
        return;
    }
}
