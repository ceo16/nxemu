#include "core_settings.h"
#include "identifiers.h"
#include "settings.h"
#include <common/json.h>
#include <common/path.h>

namespace
{
struct CoreSettingsDefaults
{
    static constexpr const char * defaultModuleDirValue = "./modules";
#ifdef _DEBUG
    static constexpr const char * defaultModuleCpu = "cpu\\nxemu-cpu_d.dll";
    static constexpr const char * defaultModuleVideo = "video\\nxemu-video_d.dll";
    static constexpr const char * defaultModuleOperatingSystem = "operating_system\\nxemu-os_d.dll";
#else
    static constexpr const char * defaultModuleCpu = "cpu\\nxemu-cpu.dll";
    static constexpr const char * defaultModuleVideo = "video\\nxemu-video.dll";
    static constexpr const char * defaultModuleOperatingSystem = "operating_system\\nxemu-os.dll";
#endif
    static constexpr bool defaultShowConsole = false;
    static constexpr bool defaultRomLoading = false;
    static constexpr bool defaultEmulationRunning = false;
    static constexpr bool defaultDisplayedFrames = false;

    static Path GetDefaultModuleDir();
};

void ModuleCpuSelectedChanged(void);
void ModuleVideoSelectedChanged(void);
void ModuleOsSelectedChanged(void);
} // namespace

CoreSettings coreSettings = {};

Path CoreSettingsDefaults::GetDefaultModuleDir()
{
    Path dir(Path::MODULE_DIRECTORY);
    dir.AppendDirectory("modules");
    return dir;
}

void LoadCoreSetting(void)
{
    Settings & settings = Settings::GetInstance();

    coreSettings = {};
    coreSettings.configDir = Path(Path(settings.GetConfigFile()).GetDriveDirectory(), "");
    coreSettings.moduleDir = CoreSettingsDefaults::GetDefaultModuleDir();
    coreSettings.moduleDirValue = CoreSettingsDefaults::defaultModuleDirValue;

    JsonValue jsonSettings = Settings::GetInstance().GetSettings("Core");
    const JsonValue * moduleDir = jsonSettings.Find("ModuleDirectory-x64");
    if (moduleDir != nullptr && moduleDir->isString())
    {
        std::string dirValue = moduleDir->asString();
        if (!dirValue.empty())
        {
            coreSettings.moduleDirValue = dirValue;
            coreSettings.moduleDir = Path(coreSettings.moduleDirValue, "").DirectoryNormalize(Path(Path::MODULE_DIRECTORY));
        }
    }

    settings.SetDefaultString(NXCoreSetting::ModuleCpuSelected, CoreSettingsDefaults::defaultModuleCpu);
    settings.SetDefaultString(NXCoreSetting::ModuleVideoSelected, CoreSettingsDefaults::defaultModuleVideo);
    settings.SetDefaultString(NXCoreSetting::ModuleOsSelected, CoreSettingsDefaults::defaultModuleOperatingSystem);
    settings.SetDefaultBool(NXCoreSetting::ShowConsole, CoreSettingsDefaults::defaultShowConsole);

    settings.SetDefaultBool(NXCoreSetting::RomLoading, CoreSettingsDefaults::defaultRomLoading);
    settings.SetDefaultBool(NXCoreSetting::EmulationRunning, CoreSettingsDefaults::defaultEmulationRunning);
    settings.SetDefaultBool(NXCoreSetting::DisplayedFrames, CoreSettingsDefaults::defaultDisplayedFrames);

    coreSettings.moduleCpuSelected = CoreSettingsDefaults::defaultModuleCpu;
    coreSettings.moduleVideoSelected = CoreSettingsDefaults::defaultModuleVideo;
    coreSettings.moduleOsSelected = CoreSettingsDefaults::defaultModuleOperatingSystem;

    JsonValue settingValue = jsonSettings["ShowConsole"];
    coreSettings.showConsole = settingValue.isBool() ? settingValue.asBool() : false;

    const JsonValue * modules = jsonSettings.Find("modules");
    if (modules != nullptr && modules->isObject())
    {
        JsonValue video = (*modules)["video"];
        if (video.isString())
        {
            coreSettings.moduleVideoSelected = video.asString();
        }
        JsonValue cpu = (*modules)["cpu"];
        if (cpu.isString())
        {
            coreSettings.moduleCpuSelected = cpu.asString();
        }
        JsonValue os = (*modules)["os"];
        if (os.isString())
        {
            coreSettings.moduleOsSelected = os.asString();
        }
    }

    settings.SetString(NXCoreSetting::ModuleVideoSelected, coreSettings.moduleVideoSelected.c_str());
    settings.SetString(NXCoreSetting::ModuleCpuSelected, coreSettings.moduleCpuSelected.c_str());
    settings.SetString(NXCoreSetting::ModuleOsSelected, coreSettings.moduleOsSelected.c_str());
    settings.SetBool(NXCoreSetting::ShowConsole, coreSettings.showConsole);
    settings.SetChanged(NXCoreSetting::ModuleVideoSelected, strcmp(coreSettings.moduleVideoSelected.c_str(), CoreSettingsDefaults::defaultModuleVideo) != 0);
    settings.SetChanged(NXCoreSetting::ModuleCpuSelected, strcmp(coreSettings.moduleCpuSelected.c_str(), CoreSettingsDefaults::defaultModuleCpu) != 0);
    settings.SetChanged(NXCoreSetting::ModuleOsSelected, strcmp(coreSettings.moduleOsSelected.c_str(), CoreSettingsDefaults::defaultModuleOperatingSystem) != 0);
    settings.SetChanged(NXCoreSetting::ShowConsole, coreSettings.showConsole != CoreSettingsDefaults::defaultShowConsole);

    Settings::GetInstance().RegisterCallback(NXCoreSetting::ModuleCpuSelected, std::bind(&ModuleCpuSelectedChanged));
    Settings::GetInstance().RegisterCallback(NXCoreSetting::ModuleVideoSelected, std::bind(&ModuleVideoSelectedChanged));
    Settings::GetInstance().RegisterCallback(NXCoreSetting::ModuleOsSelected, std::bind(&ModuleOsSelectedChanged));

    coreSettings.moduleCpuSelected = settings.GetString(NXCoreSetting::ModuleCpuSelected);
    coreSettings.moduleVideoSelected = settings.GetString(NXCoreSetting::ModuleVideoSelected);
    coreSettings.moduleOsSelected = settings.GetString(NXCoreSetting::ModuleOsSelected);
}

void SaveCoreSetting(void)
{
    JsonValue json(JsonValueType::Object);
    bool videoModuleChanged = strcmp(coreSettings.moduleVideoSelected.c_str(), CoreSettingsDefaults::defaultModuleVideo) != 0;
    bool cpuModuleChanged = strcmp(coreSettings.moduleCpuSelected.c_str(), CoreSettingsDefaults::defaultModuleCpu) != 0;
    bool osModuleChanged = strcmp(coreSettings.moduleOsSelected.c_str(), CoreSettingsDefaults::defaultModuleOperatingSystem) != 0;

    if (videoModuleChanged || cpuModuleChanged || osModuleChanged)
    {
        JsonValue modules(JsonValueType::Object);
        if (videoModuleChanged)
        {
            modules["video"] = JsonValue(coreSettings.moduleVideoSelected);
        }
        if (cpuModuleChanged)
        {
            modules["cpu"] = JsonValue(coreSettings.moduleCpuSelected);
        }
        if (osModuleChanged)
        {
            modules["os"] = JsonValue(coreSettings.moduleOsSelected);
        }
        json["modules"] = modules;
    }

    if (!coreSettings.moduleDirValue.empty())
    {
        json["ModuleDirectory-x64"] = JsonValue(coreSettings.moduleDirValue);
    }

    Settings & settings = Settings::GetInstance();
    settings.SetSettings("Core", json);
    settings.Save();
}

namespace
{
void ModuleCpuSelectedChanged(void)
{
    Settings & settings = Settings::GetInstance();
    coreSettings.moduleCpuSelected = settings.GetString(NXCoreSetting::ModuleCpuSelected);
    settings.SetChanged(NXCoreSetting::ModuleCpuSelected, strcmp(coreSettings.moduleCpuSelected.c_str(), CoreSettingsDefaults::defaultModuleCpu) != 0);
    SaveCoreSetting();
}

void ModuleVideoSelectedChanged(void)
{
    Settings & settings = Settings::GetInstance();
    coreSettings.moduleVideoSelected = settings.GetString(NXCoreSetting::ModuleVideoSelected);
    settings.SetChanged(NXCoreSetting::ModuleVideoSelected, strcmp(coreSettings.moduleVideoSelected.c_str(), CoreSettingsDefaults::defaultModuleVideo) != 0);
    SaveCoreSetting();
}

void ModuleOsSelectedChanged(void)
{
    Settings & settings = Settings::GetInstance();
    coreSettings.moduleOsSelected = settings.GetString(NXCoreSetting::ModuleOsSelected);
    settings.SetChanged(NXCoreSetting::ModuleOsSelected, strcmp(coreSettings.moduleOsSelected.c_str(), CoreSettingsDefaults::defaultModuleOperatingSystem) != 0);
    SaveCoreSetting();
}
} // namespace