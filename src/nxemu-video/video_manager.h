#pragma once
#include <nxemu-module-spec/cpu.h>
#include <nxemu-module-spec/video.h>
#include <memory>

namespace Tegra {
class MemoryManager;
}

class VideoManager :
    public IVideo
{
public:
    VideoManager(IRenderWindow & window, ISwitchSystem & system);
    ~VideoManager();

    void EmulationStarting();

    //IVideo
    bool Initialize(void) override;
    uint32_t AllocAsEx(uint64_t addressSpaceBits, uint64_t splitAddress, uint64_t bigPageBits, uint64_t pageBits) override;
    uint64_t MapBufferEx(uint64_t gpuAddr, uint64_t deviceAddr, uint64_t size, uint8_t kind, bool isBigPages) override;
    uint64_t MemoryAllocate(uint64_t size) override;
    void MemoryTrackContinuity(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid) override;
    void MemoryMap(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid, bool track) override;
    void RequestComposite(VideoFramebufferConfig * layers, uint32_t layerCount, VideoNvFence * fences, uint32_t fenceCount) override;
    uint64_t RegisterProcess(IMemory* memory) override;
    void UpdateFramebufferLayout(uint32_t width, uint32_t height) override;
    IChannelState * AllocateChannel() override;
    void PushGPUEntries(int32_t bindId, const uint64_t * commandList, uint32_t commandListSize, const uint32_t * prefetchCommandlist, uint32_t prefetchCommandlistSize);
    void ApplyOpOnDeviceMemoryPointer(const uint8_t* pointer, uint32_t* scratchBuffer, size_t scratchBufferSize, DeviceMemoryOperation operation, void* userData) override;
    bool OnCPUWrite(uint64_t addr, uint64_t size) override;

private:
    VideoManager() = delete;
    VideoManager(const VideoManager&) = delete;
    VideoManager& operator=(const VideoManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
