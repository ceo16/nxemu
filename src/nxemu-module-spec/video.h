#pragma once
#include "base.h"
#include <stdint.h>

__interface IMemory;

struct VideoFramebufferConfig 
{
    uint64_t address;
    uint32_t offset;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t pixelFormat;
    uint32_t transformFlags;
    int32_t cropLeft;
    int32_t cropTop;
    int32_t cropRight;
    int32_t cropBottom;
    uint32_t blending;
};

struct VideoNvFence
{
    int32_t id;
    uint32_t value;
};

__interface IChannelState
{
    bool Initialized() const = 0;
    int32_t BindId() const = 0;
    void Init(uint64_t programId) = 0;
    void SetMemoryManager(uint32_t id) = 0;
    void Release() = 0;
};

__interface IVideo
{
    bool Initialize(void) = 0;
    uint32_t AllocAsEx(uint64_t addressSpaceBits, uint64_t splitAddress, uint64_t bigPageBits, uint64_t pageBits) = 0;
    uint64_t MapBufferEx(uint64_t gpuAddr, uint64_t deviceAddr, uint64_t size, uint8_t kind, bool isBigPages) = 0;
    uint64_t MemoryAllocate(uint64_t size) = 0;
    void MemoryTrackContinuity(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid) = 0;
    void MemoryMap(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid, bool track) = 0;
    void RequestComposite(VideoFramebufferConfig * layers, uint32_t layerCount, VideoNvFence * fences, uint32_t fenceCount) = 0;
    uint64_t RegisterProcess(IMemory * memory) = 0;
    void UpdateFramebufferLayout(uint32_t width, uint32_t height) = 0;
    IChannelState * AllocateChannel() = 0;
    void PushGPUEntries(int32_t bindId, const uint64_t * commandList, uint32_t commandListSize, const uint32_t * prefetchCommandlist, uint32_t prefetchCommandlistSize) = 0;
};

EXPORT IVideo * CALL CreateVideo(IRenderWindow & RenderWindow, ISwitchSystem & System);
EXPORT void CALL DestroyVideo(IVideo * Video);
