#include "system_config_audio.h"

SystemConfigAudio::SystemConfigAudio(ISciterUI & sciterUI, HWINDOW parent, SciterElement page) :
    m_sciterUI(sciterUI),
    m_parent(parent),
    m_page(page)
{
}
