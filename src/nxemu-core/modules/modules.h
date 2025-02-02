#pragma once
#include "cpu_module.h"
#include "module_notification.h"
#include "module_settings.h"
#include "operating_system_module.h"
#include "video_module.h"
#include <memory>
#include <string>
#include <vector>

class Modules
{
    typedef std::vector<ModuleBase *> BaseModules;

public:
    Modules();
    ~Modules();

    bool Initialize(ISwitchSystem & System);
    void StartEmulation(void);
    void StopEmulation(void);
    IOperatingSystem * OperatingSystem(void);

private:
    Modules(const Modules &) = delete;
    Modules & operator=(const Modules &) = delete;

    void CreateModules(void);

    template <typename plugin_type>
    void LoadModule(const std::string & fileName, std::unique_ptr<plugin_type> & plugin);

    ModuleNotification m_moduleNotification;
    ModuleSettings m_moduleSettings;
    BaseModules m_baseModules;
    std::unique_ptr<CpuModule> m_cpuModule;
    std::unique_ptr<VideoModule> m_videoModule;
    std::unique_ptr<OperatingSystemModule> m_operatingsystemModule;
    IOperatingSystem * m_operatingsystem;
    std::string m_cpuFile;
    std::string m_videoFile;
    std::string m_operatingsystemFile;
};
