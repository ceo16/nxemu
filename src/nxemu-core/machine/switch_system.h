#pragma once
#include <memory>
#include "modules/modules.h"

class Nro;

class SwitchSystem
{
public:
    ~SwitchSystem();

    static bool Create();
    static void ShutDown();
    static SwitchSystem * GetInstance();

    bool LoadRom(const char * romFile);
    void StartEmulation(void);
    void StopEmulation(void);

private:
    SwitchSystem(const SwitchSystem &) = delete;
    SwitchSystem & operator=(const SwitchSystem &) = delete;

    SwitchSystem();

    bool Initialize();
    bool LoadNRO(const char * nroFile);

    static std::unique_ptr<SwitchSystem> m_instance;
    bool m_emulationRunning;
    Modules m_modules;
    std::unique_ptr<Nro> m_nro;
};
