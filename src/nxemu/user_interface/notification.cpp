#include "notification.h"
#include "settings/ui_settings.h"

std::unique_ptr<Notification> Notification::s_instance;

Notification::Notification()
{
}

void Notification::AppInitDone(void)
{
    LoadUISetting();
}

Notification & Notification::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = std::make_unique<Notification>();
    }
    return *s_instance;
}

void Notification::CleanUp()
{
    s_instance.reset();
}