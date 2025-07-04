#include "os_settings.h"
#include "os_settings_identifiers.h"
#include <common/json.h>
#include <nxemu-module-spec/base.h>
#include <yuzu_common/settings_enums.h>
#include <yuzu_common/settings.h>
#include <yuzu_common/yuzu_assert.h>

extern IModuleSettings * g_settings;

namespace
{
    enum class SettingType { String, AudioEngine, AudioMode, U8, Boolean };

    class OsSetting
    {
    public:
        OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<std::string> * val);
        OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AudioEngine> * val);
        OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AudioMode, true>* val);
        OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<u8, true> * val);
        OsSetting(const char * id, const char * section, const char * key, Settings::Setting<bool, false> * val);

        const char * identifier;
        const char * json_section;
        const char * json_key;
        SettingType settingType;
        union
        {
            Settings::SwitchableSetting<std::string> * string;
            Settings::SwitchableSetting<Settings::AudioEngine> * audioEngine;
            Settings::SwitchableSetting<Settings::AudioMode, true> * audioMode;
            Settings::SwitchableSetting<u8, true> * u8;
            Settings::Setting<bool, false> * boolean;
        } setting;
    };

    static OsSetting settings[] = {
        { NXOsSetting::AudioSinkId, "audio", "sink_id", &Settings::values.sink_id },
        { NXOsSetting::AudioOutputDeviceId, "audio", "output_device_id", &Settings::values.audio_output_device_id },
        { NXOsSetting::AudioInputDeviceId, "audio", "input_device_id", &Settings::values.audio_input_device_id },
        { NXOsSetting::AudioMode, "audio", "mode", &Settings::values.sound_index },
        { NXOsSetting::AudioVolume, "audio", "volume", &Settings::values.volume },
        { NXOsSetting::AudioMuted, "audio", "muted", &Settings::values.audio_muted },
    };
}

void OsSettingChanged(const char * setting, void * userData)
{
    for (const OsSetting & osSetting : settings)
    {
        if (strcmp(osSetting.identifier, setting) != 0)
        {
            continue;
        }
        switch (osSetting.settingType)
        {
        case SettingType::String:
            osSetting.setting.string->SetValue(g_settings->GetString(setting));
            break;
        case SettingType::AudioEngine:
            osSetting.setting.audioEngine->SetValue((Settings::AudioEngine)g_settings->GetInt(setting));
            break;
        case SettingType::AudioMode:
            osSetting.setting.audioMode->SetValue((Settings::AudioMode)g_settings->GetInt(setting));
            break;
        case SettingType::U8:
            osSetting.setting.u8->SetValue(g_settings->GetInt(setting));
            break;
        case SettingType::Boolean:
            osSetting.setting.boolean->SetValue(g_settings->GetBool(setting));
            break;
        default:
            UNIMPLEMENTED();
        }
    }
}

void SetupOsSetting(void)
{
    for (const OsSetting& osSetting : settings)
    {
        switch (osSetting.settingType)
        {
        case SettingType::String:
            osSetting.setting.string->SetValue(osSetting.setting.string->GetDefault());
            break;
        case SettingType::AudioEngine:
            osSetting.setting.audioEngine->SetValue(osSetting.setting.audioEngine->GetDefault());
            break;
        case SettingType::AudioMode:
            osSetting.setting.audioMode->SetValue(osSetting.setting.audioMode->GetDefault());
            break;
        case SettingType::U8:
            osSetting.setting.u8->SetValue(osSetting.setting.u8->GetDefault());
            break;
        case SettingType::Boolean:
            osSetting.setting.boolean->SetValue(osSetting.setting.boolean->GetDefault());
            break;
        default:
            UNIMPLEMENTED();
        }
    }
    
    JsonValue root;
    JsonReader reader;
    std::string json = g_settings->GetSectionSettings("nxemu-os");

    if (!json.empty() && reader.Parse(json.data(), json.data() + json.size(), root))
    {
        for (const OsSetting & osSetting : settings)
        {
            JsonValue section = root[osSetting.json_section];
            if (!section.isObject())
            {
                continue;
            }
            JsonValue value = section[osSetting.json_key];
            switch (osSetting.settingType)
            {
            case SettingType::String:
                if (value.isString())
                {
                    osSetting.setting.string->SetValue(value.asString());
                }
                break;
            case SettingType::AudioEngine:
                if (value.isString())
                {
                    osSetting.setting.audioEngine->SetValue(Settings::ToEnum<Settings::AudioEngine>(value.asString()));
                }
                break;
            case SettingType::AudioMode:
                if (value.isString())
                {
                    osSetting.setting.audioMode->SetValue(Settings::ToEnum<Settings::AudioMode>(value.asString()));
                }
                break;
            case SettingType::U8:
                if (value.isInt())
                {
                    osSetting.setting.u8->SetValue((uint8_t)value.asInt64());
                }
                break;
            case SettingType::Boolean:
                if (value.isBool())
                {
                    osSetting.setting.boolean->SetValue(value.asBool());
                }
                break;
            default:
                UNIMPLEMENTED();
            }
        }
    }

    for (const OsSetting& osSetting : settings)
    {
        switch (osSetting.settingType)
        {
        case SettingType::String:
            g_settings->SetDefaultString(osSetting.identifier, osSetting.setting.string->GetDefault().c_str());
            g_settings->SetString(osSetting.identifier, osSetting.setting.string->GetValue().c_str());
            break;
        case SettingType::AudioEngine:
            g_settings->SetDefaultInt(osSetting.identifier, (int32_t)osSetting.setting.audioEngine->GetDefault());
            g_settings->SetInt(osSetting.identifier, (int32_t)osSetting.setting.audioEngine->GetValue());
            break;
        case SettingType::AudioMode:
            g_settings->SetDefaultInt(osSetting.identifier, (int32_t)osSetting.setting.audioMode->GetDefault());
            g_settings->SetInt(osSetting.identifier, (int32_t)osSetting.setting.audioMode->GetValue());
            break;
        case SettingType::U8:
            g_settings->SetDefaultInt(osSetting.identifier, (int32_t)osSetting.setting.u8->GetDefault());
            g_settings->SetInt(osSetting.identifier, (int32_t)osSetting.setting.u8->GetValue());
            break;
        case SettingType::Boolean:
            g_settings->SetDefaultBool(osSetting.identifier, osSetting.setting.boolean->GetDefault() != 0);
            g_settings->SetBool(osSetting.identifier, osSetting.setting.boolean->GetValue() != 0);
            break;
        default:
            UNIMPLEMENTED();
        }
        g_settings->RegisterCallback(osSetting.identifier, OsSettingChanged, nullptr);
    }
}

void SaveOsSettings(void)
{
    typedef std::map<std::string, JsonValue> SectionMap;
    SectionMap sections;

    for (const OsSetting& osSetting : settings)
    {
        switch (osSetting.settingType)
        {
        case SettingType::String:
            if (osSetting.setting.string->GetValue() != osSetting.setting.string->GetDefault())
            {
                sections[osSetting.json_section][osSetting.json_key] = osSetting.setting.string->GetValue();
            }
            break;
        case SettingType::AudioEngine:
            if (osSetting.setting.audioEngine->GetValue() != osSetting.setting.audioEngine->GetDefault())
            {
                sections[osSetting.json_section][osSetting.json_key] = Settings::CanonicalizeEnum(osSetting.setting.audioEngine->GetValue());
            }
            break;
        case SettingType::AudioMode:
            if (osSetting.setting.audioMode->GetValue() != osSetting.setting.audioMode->GetDefault())
            {
                sections[osSetting.json_section][osSetting.json_key] = Settings::CanonicalizeEnum(osSetting.setting.audioMode->GetValue());
            }
            break;
        case SettingType::U8:
            if (osSetting.setting.u8->GetValue() != osSetting.setting.u8->GetDefault())
            {
                sections[osSetting.json_section][osSetting.json_key] = (int32_t)osSetting.setting.u8->GetValue();
            }
            break;
        case SettingType::Boolean:
            if (osSetting.setting.boolean->GetValue() != osSetting.setting.boolean->GetDefault())
            {
                sections[osSetting.json_section][osSetting.json_key] = osSetting.setting.boolean->GetValue() != 0;
            }
            break;
        default:
            UNIMPLEMENTED();
        }
    }

    JsonValue json;
    for (SectionMap::const_iterator it = sections.begin(); it != sections.end(); ++it)
    {
        if (it->second.size() > 0)
        {
            json[it->first] = it->second;
        }
    }
    g_settings->SetSectionSettings("nxemu-os", json.isNull() ? "" : JsonStyledWriter().write(json));
}

namespace
{
    OsSetting::OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<std::string> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::String)
    {
        setting.string = val;
    }

    OsSetting::OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AudioEngine> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AudioEngine)
    {
        setting.audioEngine = val;
    }

    OsSetting::OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<Settings::AudioMode, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::AudioMode)
    {
        setting.audioMode = val;
    }

    OsSetting::OsSetting(const char * id, const char * section, const char * key, Settings::SwitchableSetting<u8, true> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::U8)
    {
        setting.u8 = val;
    }
    
    OsSetting::OsSetting(const char* id, const char * section, const char * key, Settings::Setting<bool, false> * val) :
        identifier(id),
        json_section(section),
        json_key(key),
        settingType(SettingType::Boolean)
    {
        setting.boolean = val;
    }
}