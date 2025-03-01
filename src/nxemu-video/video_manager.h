#pragma once
#include <nxemu-module-spec/video.h>
#include <memory>

class VideoManager :
    public IVideo
{
public:
    VideoManager(IRenderWindow & window, ISwitchSystem & system);
    ~VideoManager();

    bool Initialize(void);

private:
    VideoManager() = delete;
    VideoManager(const VideoManager&) = delete;
    VideoManager& operator=(const VideoManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl;
};
