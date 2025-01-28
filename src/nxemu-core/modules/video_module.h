#pragma once
#include "module_base.h"

class VideoModule :
    public ModuleBase
{
public:
    VideoModule() = default;
    ~VideoModule() = default;

protected:
    void UnloadModule(void);
    bool LoadFunctions(void);
    MODULE_TYPE ModuleType() const;

private:
    VideoModule(const VideoModule &) = delete;
    VideoModule & operator=(const VideoModule &) = delete;
};