#pragma once
#include "module_base.h"
#include <nxemu-module-spec/operating_system.h>

class OperatingSystemModule :
    public ModuleBase
{
public:
    typedef IOperatingSystem *(CALL * tyCreateOperatingSystem)(ISwitchSystem & System);
    typedef void(CALL * tyDestroyOperatingSystem)(IOperatingSystem * OperatingSystem);

    OperatingSystemModule();
    ~OperatingSystemModule() = default;

    IOperatingSystem * CreateOS(ISwitchSystem & System) const
    {
        return m_CreateOS(System);
    }
    void DestroyOS(IOperatingSystem * OperatingSystem) const
    {
        m_DestroyOS(OperatingSystem);
    }

protected:
    void UnloadModule();
    bool LoadFunctions();
    MODULE_TYPE ModuleType() const;

private:
    OperatingSystemModule(const OperatingSystemModule &) = delete;
    OperatingSystemModule & operator=(const OperatingSystemModule &) = delete;

    static IOperatingSystem * CALL dummyCreateOS(ISwitchSystem & System);
    static void CALL dummyDestroyOS(IOperatingSystem * OperatingSystem);

    tyCreateOperatingSystem m_CreateOS;
    tyDestroyOperatingSystem m_DestroyOS;
};