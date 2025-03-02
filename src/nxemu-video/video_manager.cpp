#include "video_manager.h"
#include "yuzu_video_core/host1x/host1x.h"

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
        return true;
    }
    
    std::unique_ptr<Tegra::Host1x::Host1x> m_host1x;
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

