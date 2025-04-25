#pragma once
#include <memory>
#include <nxemu-core/modules/modules.h>

class SwitchSystem :
    public ISwitchSystem
{
public:
    ~SwitchSystem();

    static bool Create(IRenderWindow & window);
    static void ShutDown();
    static SwitchSystem * GetInstance();

    void StartEmulation(void);
    void StopEmulation(void);

    //ISwitchSystem
    ISystemloader & Systemloader();
    IOperatingSystem & OperatingSystem();
    IVideo & Video(void);
    ICpu & Cpu(void);

private:
    SwitchSystem(const SwitchSystem &) = delete;
    SwitchSystem & operator=(const SwitchSystem &) = delete;

    SwitchSystem();

    bool Initialize(IRenderWindow & window);

    static std::unique_ptr<SwitchSystem> m_instance;
    bool m_emulationRunning;
    Modules m_modules;
};
