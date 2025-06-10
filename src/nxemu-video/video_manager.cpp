#include "video_manager.h"
#include "render_window.h"
#include "yuzu_video_core/control/channel_state.h"
#include "yuzu_video_core/dma_pusher.h"
#include "yuzu_video_core/host1x/host1x.h"
#include "yuzu_video_core/video_core.h"
#include "yuzu_video_core/gpu.h"

struct VideoManager::Impl 
{
    Impl(IRenderWindow & window, ISwitchSystem & system) :
        m_window(window),
        m_system(system)
    {
    }

    bool Initialize(void)
    {
        m_host1x = std::make_unique<Tegra::Host1x::Host1x>(m_system.OperatingSystem().DeviceMemory());
        m_emuWindow = std::make_unique<RenderWindow>(m_window);
        m_gpuCore = VideoCore::CreateGPU(m_system, *(m_emuWindow.get()), *m_host1x);
        return true;
    }
    
    std::shared_ptr<Tegra::MemoryManager> m_gmmu;
    std::unique_ptr<Tegra::Host1x::Host1x> m_host1x;
    std::unique_ptr<RenderWindow> m_emuWindow;
    std::unique_ptr<Tegra::GPU> m_gpuCore;
    IRenderWindow & m_window;
    ISwitchSystem & m_system;
    Tegra::MemoryManagerRegistry m_memoryManagerRegistry;
};

VideoManager::VideoManager(IRenderWindow & window, ISwitchSystem & system) :
    impl{ std::make_unique<Impl>(window, system) } 
{
}

VideoManager::~VideoManager()
{
}

void VideoManager::EmulationStarting(void)
{
    impl->m_gpuCore->Start();
}

bool VideoManager::Initialize(void)
{
    return impl->Initialize();
}

void VideoManager::RequestComposite(VideoFramebufferConfig * layers, uint32_t layerCount, VideoNvFence * fences, uint32_t fenceCount)
{
    std::vector<Tegra::FramebufferConfig> output_layers;
    std::vector<Service::Nvidia::NvFence> output_fences;
    output_layers.reserve(layerCount);
    output_fences.reserve(fenceCount);

    for (uint32_t i = 0; i < layerCount; i++)
    {
        output_layers.emplace_back(Tegra::FramebufferConfig{
            .address = layers[i].address,
            .offset = layers[i].offset,
            .width = layers[i].width,
            .height = layers[i].height,
            .stride = layers[i].stride,
            .pixel_format = (Service::android::PixelFormat)layers[i].pixelFormat,
            .transform_flags = (Service::android::BufferTransformFlags)layers[i].transformFlags,
            .crop_rect = Common::Rectangle<int>{layers[i].cropLeft, layers[i].cropTop, layers[i].cropRight, layers[i].cropBottom },
            .blending = (Tegra::BlendMode)layers[i].blending,
        });
    }
    for (uint32_t i = 0; i < fenceCount; i++)
    {
        output_fences.emplace_back(Service::Nvidia::NvFence{
            .id = fences[i].id,
            .value = fences[i].value,
        });
    }
    impl->m_gpuCore->RequestComposite(std::move(output_layers), std::move(output_fences));
}

uint64_t VideoManager::RegisterProcess(IMemory * memory)
{
    Core::Asid asid = impl->m_host1x->MemoryManager().RegisterProcess(memory);
    return asid.id;
}

void VideoManager::UpdateFramebufferLayout(uint32_t width, uint32_t height)
{
    impl->m_emuWindow->UpdateCurrentFramebufferLayout(width, height);
}

IChannelState * VideoManager::AllocateChannel()
{
    return std::make_unique<IChannelStatePtr>(*impl->m_gpuCore, impl->m_memoryManagerRegistry, std::move(impl->m_gpuCore->AllocateChannel())).release();
}

void VideoManager::PushGPUEntries(int32_t bindId, const uint64_t * commandList, uint32_t commandListSize, const uint32_t * prefetchCommandlist, uint32_t prefetchCommandlistSize)
{
    Tegra::CommandList entries(commandListSize);
    memcpy(entries.command_lists.data(), commandList, sizeof(uint64_t) * commandListSize);
    if (prefetchCommandlistSize > 0)
    {
        entries.prefetch_command_list.resize(prefetchCommandlistSize);
        memcpy(entries.prefetch_command_list.data(), prefetchCommandlist, sizeof(uint32_t) * prefetchCommandlistSize);
    }
    impl->m_gpuCore->PushGPUEntries(bindId, std::move(entries));
}

uint32_t VideoManager::AllocAsEx(uint64_t addressSpaceBits, uint64_t splitAddress, uint64_t bigPageBits, uint64_t pageBits)
{
    impl->m_gmmu = std::make_shared<Tegra::MemoryManager>(impl->m_host1x->MemoryManager(), addressSpaceBits, splitAddress, bigPageBits, pageBits);
    impl->m_gpuCore->InitAddressSpace(*impl->m_gmmu);

    return impl->m_memoryManagerRegistry.AddMemoryManager(impl->m_gmmu);
}

uint64_t VideoManager::MapBufferEx(uint64_t gpuAddr, uint64_t deviceAddr, uint64_t size, uint8_t kind, bool isBigPages)
{
    return impl->m_gmmu->Map(gpuAddr, deviceAddr, size, (Tegra::PTEKind)kind, isBigPages);
}

uint64_t VideoManager::MemoryAllocate(uint64_t size)
{
    return impl->m_host1x->MemoryManager().Allocate(size);
}

void VideoManager::MemoryTrackContinuity(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid)
{
    impl->m_host1x->MemoryManager().TrackContinuity(address, virtualAddress, size, Core::Asid{ asid });
}

void VideoManager::MemoryMap(uint64_t address, uint64_t virtualAddress, uint64_t size, uint64_t asid, bool track)
{
    impl->m_host1x->MemoryManager().Map(address, virtualAddress, size, Core::Asid{asid}, track);
}
