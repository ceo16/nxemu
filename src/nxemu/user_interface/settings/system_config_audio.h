#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>

class SystemConfigAudio
{
public:
    SystemConfigAudio(ISciterUI & sciterUI, HWINDOW parent, SciterElement page);
    ~SystemConfigAudio() = default;

private:
    SystemConfigAudio() = delete;
    SystemConfigAudio(const SystemConfigAudio &) = delete;
    SystemConfigAudio & operator=(const SystemConfigAudio &) = delete;

    ISciterUI & m_sciterUI;
    HWINDOW m_parent;
    SciterElement m_page;
};