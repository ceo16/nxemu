#include "video_manager.h"
#include "render_window.h"
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
        m_gpuCore = VideoCore::CreateGPU(*(m_emuWindow.get()), *m_host1x);
        return true;
    }
    
    std::unique_ptr<Tegra::Host1x::Host1x> m_host1x;
    std::unique_ptr<RenderWindow> m_emuWindow;
    std::unique_ptr<Tegra::GPU> m_gpuCore;
    IRenderWindow & m_window;
    ISwitchSystem & m_system;
};

VideoManager::VideoManager(IRenderWindow & window, ISwitchSystem & system) :
    impl{ std::make_unique<Impl>(window, system) } 
{
}

VideoManager::~VideoManager()
{
}

bool VideoManager::Initialize(void)
{
    return impl->Initialize();
}

