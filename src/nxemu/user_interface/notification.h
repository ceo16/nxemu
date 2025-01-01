#pragma once
#include <nxemu-core/notification.h>
#include <memory>

class Notification :
    public INotification
{
public:
    Notification();

    void AppInitDone(void);

    static Notification & GetInstance();
    static void CleanUp();

private:
    Notification(const Notification&) = delete;
    Notification& operator=(const Notification&) = delete;

    static std::unique_ptr<Notification> s_instance;
};