#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <widgets/combo_box.h>

class SystemConfigAudio
{
public:
    SystemConfigAudio(ISciterUI & sciterUI, HWINDOW parent, SciterElement page);
    ~SystemConfigAudio() = default;

    void SaveSetting(void);

private:
    SystemConfigAudio() = delete;
    SystemConfigAudio(const SystemConfigAudio &) = delete;
    SystemConfigAudio & operator=(const SystemConfigAudio &) = delete;

    ISciterUI & m_sciterUI;
    HWINDOW m_parent;
    SciterElement m_page;
    std::shared_ptr<IComboBox> m_outputEngine;
};