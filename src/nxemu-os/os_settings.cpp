#include "os_settings.h"
#include "os_settings_identifiers.h"
#include <common/json.h>
#include <nxemu-module-spec/base.h>
#include <yuzu_common/settings_enums.h>
#include <yuzu_common/settings.h>

extern IModuleSettings * g_settings;

void OsSettingChanged(const char * setting, void * userData)
{
    if (strcmp(NXOsSetting::AudioSinkId, setting) == 0)
    {
        Settings::values.sink_id.SetValue((Settings::AudioEngine)g_settings->GetInt(NXOsSetting::AudioSinkId));
    }
}

void SetupOsSetting(void)
{
    std::string json = g_settings->GetSectionSettings("nxemu-os");
    Settings::values.sink_id.SetValue(Settings::values.sink_id.GetDefault());

    JsonValue root;
    JsonReader reader;
    if (!json.empty() && reader.Parse(json.data(), json.data() + json.size(), root))
    {
        JsonValue audio = root["audio"];
        if (audio.isObject())
        {
            JsonValue value = audio["sink_id"];
            if (value.isString())
            {
                Settings::values.sink_id.SetValue(Settings::ToEnum<Settings::AudioEngine>(value.asString()));
            }
        }
    }

    g_settings->SetDefaultInt(NXOsSetting::AudioSinkId, (int32_t)Settings::values.sink_id.GetDefault());
    g_settings->SetInt(NXOsSetting::AudioSinkId, (int32_t)Settings::values.sink_id.GetValue());
    g_settings->RegisterCallback(NXOsSetting::AudioSinkId, OsSettingChanged, nullptr);

}

void SaveOsSettings(void)
{
    JsonValue json_audio;
    if (Settings::values.sink_id.GetValue() != Settings::values.sink_id.GetDefault())
    {
        json_audio["sink_id"] = Settings::CanonicalizeEnum(Settings::values.sink_id.GetValue());
    }

    JsonValue json;
    if (json_audio.size() > 0)
    {
        json["audio"] = json_audio;
    }
    g_settings->SetSectionSettings("nxemu-os", json.isNull() ? "" : JsonStyledWriter().write(json));
}