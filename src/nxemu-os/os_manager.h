#pragma once
#include <nxemu-module-spec/operating_system.h>
#include <nxemu-os/core/core.h>

class OSManager :
    public IOperatingSystem
{
public:
    OSManager(ISwitchSystem & System);
    ~OSManager();

    void EmulationStarting();

    // IOperatingSystem
    bool Initialize() override;
    bool CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress, uint64_t & processID, bool is_hbl) override;
    void StartApplicationProcess(int32_t priority, int64_t stackSize, uint32_t version, StorageId baseGameStorageId, StorageId updateStorageId, uint8_t * nacpData, uint32_t nacpDataLen) override;
    bool LoadModule(const IModuleInfo & module, uint64_t baseAddress) override;
    IDeviceMemory & DeviceMemory() override;
    void KeyboardKeyPress(int modifier, int keyIndex, int keyCode) override;
    void KeyboardKeyRelease(int modifier, int keyIndex, int keyCode) override;
    void GatherGPUDirtyMemory(ICacheInvalidator * invalidator) override;
    uint64_t GetGPUTicks() override;
    void GameFrameEnd() override;
    void AudioGetSyncIDs(uint32_t* ids, uint32_t maxCount, uint32_t* actualCount) override;
    void AudioGetDeviceListForSink(uint32_t sinkId, bool capture, DeviceEnumCallback callback, void* userData) override;

private:
    OSManager() = delete;
    OSManager(const OSManager &) = delete;
    OSManager & operator=(const OSManager &) = delete;

    Core::System m_coreSystem;
    ISwitchSystem & m_switchSystem;
    Kernel::KProcess * m_process;
};
