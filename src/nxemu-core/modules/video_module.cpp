#include "video_module.h"

void VideoModule::UnloadModule(void)
{
}

bool VideoModule::LoadFunctions(void)
{
    return true;
}

MODULE_TYPE VideoModule::ModuleType() const
{
    return MODULE_TYPE_VIDEO;
}