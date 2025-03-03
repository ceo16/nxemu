#pragma once
#include <nxemu-module-spec/cpu.h>
#include <nxemu-module-spec/video.h>
#include <memory>

class VideoManager :
    public IVideo
{
public:
    VideoManager(IRenderWindow & window, ISwitchSystem & system);
    ~VideoManager();

    void EmulationStarting();

    //IVideo
    bool Initialize(void);
    uint64_t MemoryAllocate(uint64_t size);
    void MemoryTrackContinuity(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid);
    void MemoryMap(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid, bool track);
    void RequestComposite(VideoFramebufferConfig * layers, uint32_t layerCount, VideoNvFence * fences, uint32_t fenceCount);
    uint64_t RegisterProcess(IMemory* memory);

private:
    VideoManager() = delete;
    VideoManager(const VideoManager&) = delete;
    VideoManager& operator=(const VideoManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
