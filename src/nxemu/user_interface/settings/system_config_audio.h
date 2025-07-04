#pragma once
#include <sciter_ui.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <widgets/combo_box.h>

class SystemConfigAudio :
    public IStateChangeSink
{
public:
    SystemConfigAudio(ISciterUI & sciterUI, HWINDOW parent, SciterElement page);
    ~SystemConfigAudio() = default;

    void SaveSetting(void);

    // IStateChangeSink
    bool OnStateChange(SCITER_ELEMENT elem, uint32_t eventReason, void * data);
    
private:
    SystemConfigAudio() = delete;
    SystemConfigAudio(const SystemConfigAudio &) = delete;
    SystemConfigAudio & operator=(const SystemConfigAudio &) = delete;

    void updateVolumeDisplay();
    void updateAudioDevices(int32_t audioSinkId, const char * audioOutputDeviceId, const char * audioInputDeviceId);

    ISciterUI & m_sciterUI;
    HWINDOW m_parent;
    SciterElement m_page;
    std::shared_ptr<IComboBox> m_outputEngine;
    std::shared_ptr<IComboBox> m_audioOutputDevice;
    std::shared_ptr<IComboBox> m_audioInputDevice;
    std::shared_ptr<IComboBox> m_audioMode;
};