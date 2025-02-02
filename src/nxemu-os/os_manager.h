#pragma once
#include <nxemu-module-spec/operating_system.h>

class OSManager :
    public IOperatingSystem
{
public:
    OSManager(ISwitchSystem & System);
    ~OSManager();

    void EmulationStarting();

    // IOperatingSystem
    bool Initialize(void);
    bool CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress);
    void StartApplicationProcess(uint64_t baseAddress, int32_t priority, int64_t stackSize);
    bool LoadModule(const IModuleInfo & module, uint64_t baseAddress);

private:
    OSManager() = delete;
    OSManager(const OSManager &) = delete;
    OSManager & operator=(const OSManager &) = delete;

    ISwitchSystem & m_switchSystem;
};
