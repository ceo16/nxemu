#pragma once
#include "module_base.h"
#include <nxemu-module-spec/video.h>

class VideoModule :
    public ModuleBase
{
public:
    typedef IVideo *(CALL * tyCreateVideo)(IRenderWindow & RenderWindow, ISwitchSystem & System);
    typedef void(CALL * tyDestroyVideo)(IVideo * Video);

    VideoModule();
    ~VideoModule() = default;

    IVideo * CreateVideo(IRenderWindow & RenderWindow, ISwitchSystem & System) const
    {
        return m_CreateVideo(RenderWindow, System);
    }
    void DestroyVideo(IVideo * Video) const
    {
        m_DestroyVideo(Video);
    }

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);
    MODULE_TYPE ModuleType() const;

private:
    VideoModule(const VideoModule &) = delete;
    VideoModule & operator=(const VideoModule &) = delete;

    static IVideo * CALL dummyCreateVideo(IRenderWindow & RenderWindow, ISwitchSystem & System);
    static void CALL dummyDestroyVideo(IVideo * Video);

    tyCreateVideo m_CreateVideo;
    tyDestroyVideo m_DestroyVideo;
};