#pragma once
#include <nxemu-module-spec/system_loader.h>
#include <memory>

class Nro;

class Systemloader :
    public ISystemloader
{
public:
    Systemloader(ISwitchSystem & system);
    ~Systemloader();

    //ISystemloader
    bool SelectAndLoad(void * parentWindow);
    bool LoadRom(const char * romFile);

private:
    Systemloader() = delete;
    Systemloader(const Systemloader &) = delete;
    Systemloader & operator=(const Systemloader &) = delete;

    bool LoadNRO(const char * nroFile);

    ISwitchSystem & m_system;
    std::unique_ptr<Nro> m_nro;
    uint64_t m_TitleID;
};
