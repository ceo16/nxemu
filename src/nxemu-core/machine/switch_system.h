#pragma once
#include <memory>

class Nro;

class SwitchSystem
{
public:
    ~SwitchSystem() = default;

    static bool Create();
    static void ShutDown();
    static SwitchSystem * GetInstance();

    bool LoadRom(const char * romFile);
    void StartEmulation(void);

private:
    SwitchSystem(const SwitchSystem &) = delete;
    SwitchSystem & operator=(const SwitchSystem &) = delete;

    SwitchSystem();

    bool Initialize();
    bool LoadNRO(const char * nroFile);

    static std::unique_ptr<SwitchSystem> m_instance;
    std::unique_ptr<Nro> m_nro;
};
