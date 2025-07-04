#include "system_config_audio.h"
#include "settings/ui_settings_identifiers.h"
#include <common/std_string.h>
#include <nxemu-core/settings/settings.h>
#include <widgets/combo_box.h>
#include <yuzu_common/settings_enums.h>
#include <nxemu-core/machine/switch_system.h>
#include <nxemu-os/os_settings_identifiers.h>

namespace
{
void AddDeviceToVector(const char * device, void * userData)
{
    std::vector<std::string> * deviceList = (std::vector<std::string> *)userData;
    deviceList->emplace_back(device);
}
}

SystemConfigAudio::SystemConfigAudio(ISciterUI & sciterUI, HWINDOW parent, SciterElement page) :
    m_sciterUI(sciterUI),
    m_parent(parent),
    m_page(page)
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system == nullptr)
    {
        return;
    }
    IOperatingSystem & OperatingSystem = system->OperatingSystem();

    uint32_t count = 0;
    OperatingSystem.AudioGetSyncIDs(nullptr, 0, &count);
    std::vector<uint32_t> ids(count);
    OperatingSystem.AudioGetSyncIDs(ids.data(), count, &count);

    SettingsStore & settings = SettingsStore::GetInstance();
    int32_t audioSinkId = settings.GetInt(NXOsSetting::AudioSinkId);
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(page.GetElementByID("audioOutputEngine"), IID_ICOMBOBOX);
    if (interfacePtr)
    {
        m_outputEngine = std::static_pointer_cast<IComboBox>(interfacePtr);
        int32_t index = m_outputEngine->AddItem("auto", stdstr_f("%d", Settings::AudioEngine::Auto).c_str());
        int32_t selectedIndex = index;
        for (size_t i = 0, n = ids.size(); i < n; i++)
        {
            index = m_outputEngine->AddItem(Settings::CanonicalizeEnum((Settings::AudioEngine)ids[i]).c_str(), stdstr_f("%d", ids[i]).c_str());
            if (audioSinkId == (int32_t)ids[i])
            {
                selectedIndex = index;
            }
        }
        if (selectedIndex >= 0)
        {
            m_outputEngine->SelectItem(selectedIndex);
        }
    }
    int32_t audioMode = settings.GetInt(NXOsSetting::AudioMode);
    interfacePtr = m_sciterUI.GetElementInterface(page.GetElementByID("soundMode"), IID_ICOMBOBOX);
    if (interfacePtr)
    {
        std::array modes = { Settings::AudioMode::Mono, Settings::AudioMode::Stereo, Settings::AudioMode::Surround };
        m_audioMode = std::static_pointer_cast<IComboBox>(interfacePtr);
        int32_t selectedIndex = -1;
        for (size_t i = 0, n = modes.size(); i < n; i++)
        {
            int32_t index = m_audioMode->AddItem(Settings::CanonicalizeEnum(modes[i]).c_str(), stdstr_f("%d", modes[i]).c_str());
            if (audioMode == (int32_t)modes[i])
            {
                selectedIndex = index;
            }
        }
        if (selectedIndex >= 0)
        {
            m_audioMode->SelectItem(selectedIndex);
        }
    }
    SciterElement element = page.GetElementByID("audioVolume");
    if (element)
    {
        element.SetValue(SciterValue(settings.GetInt(NXOsSetting::AudioVolume)));
        updateVolumeDisplay();
    }
    element = page.GetElementByID("muteAudio");
    if (element)
    {
        bool checked = settings.GetBool(NXOsSetting::AudioMuted);
        element.SetState(checked ? SciterElement::STATE_CHECKED : 0, checked ? 0 : SciterElement::STATE_CHECKED, true);
    }
    element = page.GetElementByID("muteBackground");
    if (element)
    {
        bool checked = settings.GetBool(UiSetting::AudioMuteWhenInBackground);
        element.SetState(checked ? SciterElement::STATE_CHECKED : 0, checked ? 0 : SciterElement::STATE_CHECKED, true);
    }
    const char * audioOutputDeviceId = settings.GetString(NXOsSetting::AudioOutputDeviceId);
    const char * audioInputDeviceId = settings.GetString(NXOsSetting::AudioInputDeviceId);
    updateAudioDevices(audioSinkId, audioOutputDeviceId, audioInputDeviceId);
    m_sciterUI.AttachHandler(page.GetElementByID("audioOutputEngine"), IID_ISTATECHANGESINK, (IStateChangeSink*)this);
    m_sciterUI.AttachHandler(page.GetElementByID("audioVolume"), IID_ISTATECHANGESINK, (IStateChangeSink*)this);
}

void SystemConfigAudio::SaveSetting(void)
{
    SettingsStore & settings = SettingsStore::GetInstance();
    SciterElement element = m_outputEngine->GetSelectedItem();
    if (element)
    {
        std::string value = element.GetAttribute("value");
        if (value.size() > 0)
        {
            settings.SetInt(NXOsSetting::AudioSinkId, std::stoi(value));
        }
    }
    element = m_audioOutputDevice->GetSelectedItem();
    if (element)
    {
        std::string value = element.GetAttribute("value");
        if (value.size() > 0)
        {
            settings.SetString(NXOsSetting::AudioOutputDeviceId, value.c_str());
        }
    }
    element = m_audioInputDevice->GetSelectedItem();
    if (element)
    {
        std::string value = element.GetAttribute("value");
        if (value.size() > 0)
        {
            settings.SetString(NXOsSetting::AudioInputDeviceId, value.c_str());
        }
    }
    element = m_audioMode->GetSelectedItem();
    if (element)
    {
        std::string value = element.GetAttribute("value");
        if (value.size() > 0)
        {
            settings.SetInt(NXOsSetting::AudioMode, std::stoi(value));
        }
    }
    element = m_page.GetElementByID("audioVolume");
    if (element)
    {
        SciterValue value = element.GetValue();
        if (value.isInt())
        {
            settings.SetInt(NXOsSetting::AudioVolume, value.GetValueInt());
        }
    }
    element = m_page.GetElementByID("muteAudio");
    if (element)
    {
        settings.SetBool(NXOsSetting::AudioMuted, (element.GetState() & SciterElement::STATE_CHECKED) != 0);
    }
}

bool SystemConfigAudio::OnStateChange(SCITER_ELEMENT elem, uint32_t /*eventReason*/, void * /*data*/)
{
    if (m_page.GetElementByID("audioOutputEngine") == elem)
    {
        SciterElement element = m_outputEngine->GetSelectedItem();
        if (!element)
        {
            return false;
        }
        std::string value = element.GetAttribute("value");
        if (value.empty())
        {
            return false;
        }
        int audioSinkId = std::stoi(value);
        SettingsStore & settings = SettingsStore::GetInstance();
        const char * audioOutputDeviceId = settings.GetDefaultString(NXOsSetting::AudioOutputDeviceId);
        const char * audioInputDeviceId = settings.GetDefaultString(NXOsSetting::AudioInputDeviceId);
        updateAudioDevices(audioSinkId, audioOutputDeviceId, audioInputDeviceId);
    }
    else if (m_page.GetElementByID("audioVolume") == elem)
    {
        updateVolumeDisplay();
    }
    return false;
}

void SystemConfigAudio::updateVolumeDisplay()
{
    SciterElement audioVolume = m_page.GetElementByID("audioVolume");
    SciterElement volumeDisplay = m_page.GetElementByID("VolumeDisplay");

    if (audioVolume && volumeDisplay)
    {
        SciterValue value = audioVolume.GetValue();
        if (value.isInt())
        {
            stdstr_f text("%d %%", value.GetValueInt());
            volumeDisplay.SetHTML((const uint8_t*)text.c_str(), text.size());
        }
    }
}

void SystemConfigAudio::updateAudioDevices(int32_t audioSinkId, const char * audioOutputDeviceId, const char * audioInputDeviceId)
{
    SwitchSystem * system = SwitchSystem::GetInstance();
    if (system == nullptr)
    {
        return;
    }
    IOperatingSystem & OperatingSystem = system->OperatingSystem();
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(m_page.GetElementByID("audioOutputDevice"), IID_ICOMBOBOX);
    if (interfacePtr)
    {
        m_audioOutputDevice = std::static_pointer_cast<IComboBox>(interfacePtr);
        m_audioOutputDevice->ClearContents();
        std::vector<std::string> devices;
        OperatingSystem.AudioGetDeviceListForSink(audioSinkId, false, AddDeviceToVector, &devices);
        int32_t index = m_audioOutputDevice->AddItem("auto", "auto");
        int32_t selectedIndex = index;
        for (size_t i = 0, n = devices.size(); i < n; i++)
        {
            index = m_audioOutputDevice->AddItem(devices[i].c_str(), devices[i].c_str());
            if (strcmp(audioOutputDeviceId, devices[i].c_str()) == 0)
            {
                selectedIndex = index;
            }
        }
        if (selectedIndex >= 0)
        {
            m_audioOutputDevice->SelectItem(selectedIndex);
        }
    }

    interfacePtr = m_sciterUI.GetElementInterface(m_page.GetElementByID("audioInputDevice"), IID_ICOMBOBOX);
    if (interfacePtr)
    {
        m_audioInputDevice = std::static_pointer_cast<IComboBox>(interfacePtr);
        m_audioInputDevice->ClearContents();
        std::vector<std::string> devices;
        OperatingSystem.AudioGetDeviceListForSink(audioSinkId, true, AddDeviceToVector, &devices);
        int32_t index = m_audioInputDevice->AddItem("auto", "auto");
        int32_t selectedIndex = index;
        for (size_t i = 0, n = devices.size(); i < n; i++)
        {
            index = m_audioInputDevice->AddItem(devices[i].c_str(), devices[i].c_str());
            if (strcmp(audioInputDeviceId, devices[i].c_str()) == 0)
            {
                selectedIndex = index;
            }
        }
        if (selectedIndex >= 0)
        {
            m_audioInputDevice->SelectItem(selectedIndex);
        }
    }
}
