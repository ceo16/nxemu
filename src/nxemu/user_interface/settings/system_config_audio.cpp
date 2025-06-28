#include "system_config_audio.h"
#include <common/std_string.h>
#include <nxemu-core/settings/settings.h>
#include <widgets/combo_box.h>
#include <yuzu_common/settings_enums.h>
#include <nxemu-core/machine/switch_system.h>
#include <nxemu-os/os_settings_identifiers.h>

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
    int32_t AudioSinkId = settings.GetInt(NXOsSetting::AudioSinkId);
    std::shared_ptr<void> interfacePtr = m_sciterUI.GetElementInterface(page.GetElementByID("audioOutputEngine"), IID_ICOMBOBOX);
    if (interfacePtr)
    {
        m_outputEngine = std::static_pointer_cast<IComboBox>(interfacePtr);
        int32_t index = m_outputEngine->AddItem("auto", stdstr_f("%d", Settings::AudioEngine::Auto).c_str());
        int32_t selectedIndex = -1;
        if (AudioSinkId == (int32_t)Settings::AudioEngine::Auto)
        {
            selectedIndex = index;
        }
        for (size_t i = 0, n = ids.size(); i < n; i++)
        {
            index = m_outputEngine->AddItem(Settings::CanonicalizeEnum((Settings::AudioEngine)ids[i]).c_str(), stdstr_f("%d", ids[i]).c_str());
            if (AudioSinkId == (int32_t)ids[i])
            {
                selectedIndex = index;
            }
        }
        if (selectedIndex >= 0)
        {
            m_outputEngine->SelectItem(selectedIndex);
        }
    }
}

void SystemConfigAudio::SaveSetting(void)
{
    SettingsStore & settings = SettingsStore::GetInstance();
    SciterElement outputEngine = m_outputEngine->GetSelectedItem();
    if (outputEngine)
    {
        std::string value = outputEngine.GetAttribute("value");
        if (value.size() > 0)
        {
            settings.SetInt(NXOsSetting::AudioSinkId, std::stoi(value));
        }
    }
}
